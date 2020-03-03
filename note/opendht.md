[TOC]

# Explore the operations in OpenDHT

## A brute force way to wait for the searching operation

We could easily put a outer vatiable inside the callback lambda, which is one of the parameters that `DhtRunner` receives. Actually, this is the spin lock. 

Further investigation: modify the lock mechanism to conditional variable rather than spin lock, to reduce the computation load. 

```cpp
#include <opendht.h>
#include <vector>
#include <thread>

int main() {
	dht::DhtRunner node;
	node.run(4222, dht::crypto::generateIdentity(), true);

	node.bootstrap("bootstrap.jami.net", "4222");

	bool finished = false;

    node.get(dht::InfoHash::get("123"), [](const std::vector<std::shared_ptr<dht::Value>>& values) {
        // Callback called when values are found
        for (const auto& value : values)
            std::cout << "Found value: " << *value << std::endl;
        return true; // return false to stop the search
    }, 
    [&finished](bool success) {
        std::cout << "Get finished with " << (success ? "success" : "failure") << std::endl;
		finished = true;
    });

	while (!finished);
	node.join();
	return 0;
}
```

## Fetch and parse the storage data

By looking through the documentation on [API Overview](https://github.com/savoirfairelinux/opendht/wiki/API-Overview), I find that the datastructure `Value` is exactly the place where the user input data resides. Notice that the struct `Value` has overload the operator `<<` based on the observation on the code above. And here is the [code](https://github.com/savoirfairelinux/opendht/blob/master/src/value.cpp#L45) about the overload detail: 

```cpp
std::ostream& operator<< (std::ostream& s, const Value& v)
{
    auto flags(s.flags());
    s << "Value[id:" << std::hex << v.id << std::dec << ' ';
    if (v.isEncrypted())
        s << "encrypted ";
    else if (v.isSigned()) {
        s << "signed (v" << v.seq << ") ";
        if (v.recipient)
            s << "decrypted ";
    }
    if (not v.isEncrypted()) {
        if (v.type == IpServiceAnnouncement::TYPE.id) {
            s << IpServiceAnnouncement(v.data);
        } else if (v.type == CERTIFICATE_TYPE.id) {
            s << "Certificate";
#ifdef OPENDHT_LOG_CRT_ID
            try {
                auto h = crypto::Certificate(v.data).getPublicKey().getLongId();
                s << " with ID " << h;
            } catch (const std::exception& e) {
                s << " (invalid)";
            }
#endif
        } else {
            if (v.user_type.empty())
                s << "data:";
            else
                s << "data(" << v.user_type << "):";
            if (v.user_type == "text/plain") {
                s << '"';
                s.write((const char*)v.data.data(), v.data.size());
                s << '"';
            } else {
                s << std::hex;
                for (auto i : v.data)
                    s << std::setfill('0') << std::setw(2) << (unsigned)i;
                s << std::dec;
            }
        }
    }
    s << ']';
    s.flags(flags);
    return s;
}
```

Noticed that there're several data sections specified by the Value struct. By going into the definition, I find that these are [`using Blob = std::vector<uint8_t>` and `typedef unsigned char uint8_t`]:

*   `Blob data;`
*   `std::string user_type;`
*   `Blob signature;`
*   `Blob cypher;`

Thus, we can directly get the storage data from Value by calling the built-in method `data()`. Here is an example: 

```cpp
node.get(dht::InfoHash::get("123"), [](const std::vector<std::shared_ptr<dht::Value>>& values) {
    // Callback called when values are found
    for (const auto& value : values) {
        std::cout << "Found value: " << *value << std::endl;
        std::cout << (*value).data.data() << std::endl;
        auto variable = (*value).data.data();
        std::cout << (char*)(variable) << std::endl;
    }
    return true; // return false to stop the search
}, 
[&finished](bool success) {
    std::cout << "Get finished with " << (success ? "success" : "failure") << std::endl;
    finished = true;
});
```

Here the data type of the `variable` is `unsigned char*`. And we can get the length of the string by calling `std::strlen((char*)variable)`. That's because the method `std::strlen` is designed for the `char *` type in C language. 