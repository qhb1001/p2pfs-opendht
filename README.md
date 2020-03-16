# p2pfs-opendht
A P2P File System designed for OpenDHT

In this project, we are going to implement Neat and Brilliant File System (NBFS) for the Peer-to-Peer file sharing system. We will implement the P2P system with OpenDHT. After that, we will provide file system support for the P2P system, so that user could easily look through the files stored in the remote peer by mounting the file system. 

# Running

You can simply pull the container we have pushed to the Docker hub to access the prepared environment by following command (you can find the Dockerfile in this repository): 

```cmd
docker pull hongbo1001/fuse
```

before the fuse system was installed in your docker. We should implement the system of opendht

```sh
docker run -d -p4222:4222/udp aberaud/opendht dhtnode -b bootstrap.jami.net -p 4222 -s
```

In order to grant the fuse enough privilege to mount the file system, the following command would be necessary: 

```sh
docker run -it -e AWSACCESSKEYID=obscured -e AWSSECRETACCESSKEY=obscured --privileged hongbo1001/fuse bash
```

While after we mount the system, the process will be running in the first terminal. And it is the time to start another terminal to access the mount file. The formal command to accomplish the task above is below:

```sh
docker exec -it  -e AWSACCESSKEYID=obscured -e AWSSECRETACCESSKEY=obscured --privileged [container-id] bash
```

#Requirements for Testing 

- Cmake2.8.8 or higher for google test
    For ubuntu/Linux not installed cmake will need to running this command

```
wget https://github.com/Kitware/CMake/releases/download/v3.17.0-rc2/cmake-3.17.0-rc2-Linux-x86_64.sh
```

Google Test Command Line 

```sh
cmake . -DCMAKE_CXX_FLAGS:STRING=-std=c++11
make 
```

if you have this error please edit googletest/CMakeList.txt line 13 edit 

```
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.1")
```

```
if (CMAKE_VERSION VERSION_GREATER "3.0.2")

```

For any docker cannot strated in macbook we should run this toolbox command. 

```sh
bash --login '/Applications/Docker/Docker Quickstart Terminal.app/Contents/Resources/Scripts/start.sh'
```

# Testing 

Add testing for google test in subfolder.

```
git submodule add https://github.com/google/googletest.git
```



# TODO

*   Create the directory support for the current design 
*   Modify the structure of the path from '/' to ':'
*   Design the client-end program to upload the files and directories

