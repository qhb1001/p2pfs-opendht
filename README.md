# p2pfs-opendht
A P2P File System designed for OpenDHT

In this project, we are going to implement Neat and Brilliant File System (NBFS) for the Peer-to-Peer file sharing system. We will implement the P2P system with OpenDHT. After that, we will provide file system support for the P2P system, so that user could easily look through the files stored in the remote peer by mounting the file system. 

#Requirements
- Cmake2.8.8 or higher for google test

Google Test Command Line 

```
cmake . -DCMAKE_CXX_FLAGS:STRING=-std=c++11
make 
```

# Testing 
Add testing for google test in subfolder.

```
git submodule add https://github.com/google/googletest.git
```

# Running

You can simply pull the container we have pushed to the Docker hub to access the prepared environment by following command (you can find the Dockerfile in this repository): 

```cmd
docker pull hongbo1001/fuse
```

In order to grant the fuse enough privilege to mount the file system, the following command would be necessary: 

```sh
docker run -it -e AWSACCESSKEYID=obscured -e AWSSECRETACCESSKEY=obscured --privileged hongbo1001/fuse sh
```

While after we mount the system, the process will be running in the first terminal. And it is the time to start another terminal to access the mount file. The formal command to accomplish the task above is below:

```sh
docker exec -it  -e AWSACCESSKEYID=obscured -e AWSSECRETACCESSKEY=obscured --privileged [container-id] bash
```

# TODO

*   ~~Create the fuse environment for a docker image~~
*   ~~Migrate the installation command from the existing docker image to my own~~
*   ~~Practice the basic node join, put and get operation~~
*   Develop the client to conveniently upload files and implement registration process
*   Develop the file system to implement the data CRUD operations. 

The two process above would be realized by Python. That's because I encounter some kind of bug when calling the C++ API provided by OpenDHT to fetch data from remote node. While at the same time, Python API works much smoothly. 

*   Try to integrate the openDHT node into FUSE program

