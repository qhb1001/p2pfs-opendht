[TOC]



# FUSE Documentation

## [Required FUSE Functions](https://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201109/homework/fuse/fuse_doc.html#gotchas)

### `void* init(struct fuse_conn_info *conn)`

This is the place where I should put the openDHT node initialization. And I'll use the `private_data` field in the structure `fuse_context` as the openDHT node entry. 

>   Initialize the filesystem. This function can often be left unimplemented, but it can be a handy way to perform one-time setup such as allocating variable-sized data structures or initializing a new filesystem. The `fuse_conn_info` structure gives information about what features are supported by FUSE, and can be used to request certain capabilities (see [below](http://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201109/homework/fuse/fuse_doc.html#init-args) for more information). The return value of this function is available to all file operations in the `private_data` field of `fuse_context`. It is also passed as a parameter to the destroy() method. (Note: see the warning under [Other Options](http://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201109/homework/fuse/fuse_doc.html#other-options) below, regarding relative pathnames.)

### `getattr(const char* path, struct stat* stbuf)`

>   Return file attributes. The "stat" structure is described in detail in the stat(2) manual page. For the given pathname, this should fill in the elements of the "stat" structure. If a field is meaningless or semi-meaningless (e.g., `st_ino`) then it should be set to 0 or given a "reasonable" value. This call is pretty much required for a usable filesystem.

The result of running `man 2 stat` command in Linux environment 

>   The stat() function obtains information about the file pointed to by path. Read, write or execute permission of the named file is not required, but all directories listed in the path name leading to the file must be searchable.
>
>   The buf argument is a pointer to a stat structure as defined by <sys/stat.h> and into which information is placed concerning the file.
>
>   When the macro _DARWIN_FEATURE_64_BIT_INODE is not defined (see below for more information about this macro), the stat structure is defined as:
>
>   ```c
>        struct stat { /* when _DARWIN_FEATURE_64_BIT_INODE is NOT defined */
>            dev_t    st_dev;    /* device inode resides on */
>            ino_t    st_ino;    /* inode's number */
>            mode_t   st_mode;   /* inode protection mode */
>            nlink_t  st_nlink;  /* number of hard links to the file */
>            uid_t    st_uid;    /* user-id of owner */
>            gid_t    st_gid;    /* group-id of owner */
>            dev_t    st_rdev;   /* device type, for special file inode */
>            struct timespec st_atimespec;  /* time of last access */
>            struct timespec st_mtimespec;  /* time of last data modification */
>            struct timespec st_ctimespec;  /* time of last file status change */
>            off_t    st_size;   /* file size, in bytes */
>            quad_t   st_blocks; /* blocks allocated for file */
>            u_long   st_blksize;/* optimal file sys I/O ops blocksize */
>            u_long   st_flags;  /* user defined flags for file */
>            u_long   st_gen;    /* file generation number */
>        };
>   ```
>
>   

### `readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)`

>   Return one or more directory entries (`struct dirent`) to the caller. This is one of the most complex FUSE functions. It is related to, but not identical to, the readdir(2) and getdents(2) system calls, and the readdir(3) library function. Because of its complexity, it is described separately [below](http://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201109/homework/fuse/fuse_doc.html#readdir-details). Required for essentially any filesystem, since it's what makes `ls` and a whole bunch of other things work.

### `mkdir(const char* path, mode_t mode)`

>   Create a directory with the given name. The directory permissions are encoded in `mode`. See mkdir(2) for details. This function is needed for any reasonable read/write filesystem.

### `rename(const char* from, const char* to)`

>   Rename the file, directory, or other object "`from`" to the target "`to`". Note that the source and target don't have to be in the same directory, so it may be necessary to move the source to an entirely new directory. See rename(2) for full details.

### `read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi)`

>   Read `size` bytes from the given file into the buffer `buf`, beginning `offset` bytes into the file. See read(2) for full details. Returns the number of bytes transferred, or 0 if `offset` was at or beyond the end of the file. Required for any sensible filesystem.

## [Mount & Unmount](https://developer.ibm.com/technologies/linux/tutorials/l-lpic1-104-3/#umount)

To mount a file system, we can directly call the excutable file with the given mount point, like `./test-fuse testdir`. In this case, the file system will be run in the background. And the log info will not be visible. 

We can run gdb to get all the helpful runtime info by following the process below: 

```cmd
$ mkdir testdir         # if necessary
$ gdb hello
[gdb noise]
(gdb) [set breakpoints, etc.]
(gdb) run -s -d -f testdir
```

flag `-s` means single thread; flag `-d` means in debug mode; `-f` means direct the print pipe to stdout.

And we can unmount the file system by running `umount testdir`. Notice that even if we kill the process, the command is still necessary to be executed. 

# P2P File System Design Documentation

## User Namespace 

In order to create different namespace for the system user, and to properly regulate the files, I decide to append the prefix string to every file name in FUSE and OpenDHT. The prefix could be set by the predefined command like `nbfs login "username"`, which stores the string in the OpenDHT or the local file. 

# C/CPP fundamental knowledge

1.  In C, the type of the hard coded string or the return value of `std::string.c_str()` is `const char *`, which is different from `char *`. 

    My first version of the loging function is `log_msg(char * format, ...)`, which should be modified to `log_msg(const char *format, ...)`. 

    [reference](https://stackoverflow.com/questions/20944784/why-is-conversion-from-string-constant-to-char-valid-in-c-but-invalid-in-c)

2.  

# ~~Embedding Python in C Program~~

According to the development note in README.md, we are going to adopt python library as the interactive window between FUSE and OpenDHT. Then, the main problem lies at the correct way to communicate C program with Python program. 

## ~~`gcc\g++` Compiler Flags~~

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

## ~~[Add Python System Path for User-defined Module](https://stackoverflow.com/questions/24492327/python-embedding-in-c-importerror-no-module-named-pyfunction/35582046)~~

The first step to execute python function in C program would be to import modules or packages. However, since we are developing with not IDE or virtual environment, which will automatically add the current path to the system path, we need to add the path to the system path. 

One way to do this is to hard code the command to execute the file as follows

```
 $ PYTHONPATH=. ./call_function pyfunction multiply 2 3
```

While we can also add the path by `sys.path.insert(0, "./path/to/your/modules/")` in the C program. That's because the system module has already been there for the python interpreter. 

