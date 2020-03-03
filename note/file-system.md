# Embedding Python in C Program

According to the development note in README.md, we are going to adopt python library as the interactive window between FUSE and OpenDHT. Then, the main problem lies at the correct way to communicate C program with Python program. 

## `gcc\g++` Compiler Flags

In the Python official documentation, they have already developed the header files for C. On Unix, these are located in the directories `*prefix*/include/pythonversion/` and `*exec_prefix*/include/pythonversion/`, where `prefix` and `exec_prefix` are defined by the corresponding parameters to Python’s **configure** script and *version* is `'%d.%d' % sys.version_info[:2]`.

While I don't know where to look for the Python's configure script, I run `grep -r '/include/python' .` at the root directory and find the path where header files resides is `/usr/include/python3.5m`. 

Though afterwards, I find the link to get the exact cflags to compile the c source code at [here](https://docs.python.org/3/extending/embedding.html#compiling-and-linking-under-unix-like-systems). And by making use of `grep` command, I find the python3-config file is located at `/usr/bin/python3-config`. The rseult of command `/usr/bin/python3-config --cflags` is below: 

```cmd
-I/usr/include/python3.5m -I/usr/include/python3.5m  -Wno-unused-result -Wsign-compare -g -fstack-protector-strong -Wformat -Werror=format-security  -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes
```

Likewise, the link path is also necessary, here is the result of executing the command `/usr/bin/python3-config -- ldflags`

```cmd
-L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu -L/usr/lib -lpython3.5m -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions
```

## [Add Python System Path for User-defined Module](https://stackoverflow.com/questions/24492327/python-embedding-in-c-importerror-no-module-named-pyfunction/35582046)

The first step to execute python function in C program would be to import modules or packages. However, since we are developing with not IDE or virtual environment, which will automatically add the current path to the system path, we need to add the path to the system path. 

One way to do this is to hard code the command to execute the file as follows

```
 $ PYTHONPATH=. ./call_function pyfunction multiply 2 3
```

While we can also add the path by `sys.path.insert(0, "./path/to/your/modules/")` in the C program. That's because the system module has already been there for the python interpreter. 

