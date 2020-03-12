/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>
  Copyright (C) 2011       Sebastian Pipping <sebastian@pipping.org>
  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

/** @file

 *
 * Compile with
 *
 *     gcc -Wall passthrough.c `pkg-config fuse3 --cflags --libs` -o passthrough
 *      gcc -Wall fuse-ramfs.c `pkg-config fuse3 --cflags --libs` -o fuse-ramfs 
 * ## Source code ##
 * \include passthrough.c
 */


#define FUSE_USE_VERSION 31

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _GNU_SOURCE

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#ifdef __FreeBSD__
#include <sys/socket.h>
#include <sys/un.h>
#endif
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#include "passthrough_helpers.h"

static void *nbfs_init(struct fuse_conn_info *conn,
		      struct fuse_config *cfg)
{
	(void) conn;
	cfg->use_ino = 1;

	/* Pick up changes from lower filesystem right away. This is
	   also necessary for better hardlink support. When the kernel
	   calls the unlink() handler, it does not know the inode of
	   the to-be-removed entry and can therefore not invalidate
	   the cache of the associated inode - resulting in an
	   incorrect st_nlink value being reported for any remaining
	   hardlinks to this inode. */
	cfg->entry_timeout = 0;
	cfg->attr_timeout = 0;
	cfg->negative_timeout = 0;

	return NULL;
}

static int nbfs_getattr(const char *path, struct stat *stbuf,  //OK 
		       struct fuse_file_info *fi)
{
	(void) fi;
	int res;

	res = lstat(path, stbuf);      // lstat()获取文件属性 传给 stbuf   已录入问题解决 
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_access(const char *path, int mask)
{
	int res;

	res = access(path, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_readlink(const char *path, char *buf, size_t size)    // 
{
	int res;

	res = readlink(path, buf, size - 1);   //将path 的符号链接传给 buf 
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int nbfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,   // 
		       off_t offset, struct fuse_file_info *fi,
		       enum fuse_readdir_flags flags)
{
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;
	(void) flags;

	dp = opendir(path);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0, 0))     //读取目录 
			break;
	}

	closedir(dp);
	return 0;
}

static int nbfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;

	res = mknod_wrapper(AT_FDCWD, path, NULL, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_mkdir(const char *path, mode_t mode)  //OK 
{
	int res;

	res = mkdir(path, mode);   //已录入问题解决 
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_unlink(const char *path)   //unlink() 删除文件 
{
	int res;

	res = unlink(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_rmdir(const char *path) // OK 
{
	int res;

	res = rmdir(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_symlink(const char *from, const char *to)
{
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_rename(const char *from, const char *to, unsigned int flags)
{
	int res;

	if (flags)
		return -EINVAL;

	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_link(const char *from, const char *to)
{
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_chmod(const char *path, mode_t mode,
		     struct fuse_file_info *fi)
{
	(void) fi;
	int res;

	res = chmod(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_chown(const char *path, uid_t uid, gid_t gid,
		     struct fuse_file_info *fi)
{
	(void) fi;
	int res;

	res = lchown(path, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_truncate(const char *path, off_t size,
			struct fuse_file_info *fi)
{
	int res;

	if (fi != NULL)
		res = ftruncate(fi->fh, size);
	else
		res = truncate(path, size);
	if (res == -1)
		return -errno;

	return 0;
}

#ifdef HAVE_UTIMENSAT
static int nbfs_utimens(const char *path, const struct timespec ts[2],
		       struct fuse_file_info *fi)
{
	(void) fi;
	int res;

	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;

	return 0;
}
#endif

static int nbfs_create(const char *path, mode_t mode,
		      struct fuse_file_info *fi)
{
	int res;

	res = open(path, fi->flags, mode);
	if (res == -1)
		return -errno;

	fi->fh = res;
	return 0;
}

static int nbfs_open(const char *path, struct fuse_file_info *fi)  
{
	int res;

	res = open(path, fi->flags);
	if (res == -1)
		return -errno;

	fi->fh = res;
	return 0;
}

static int nbfs_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd;
	int res;

	if(fi == NULL)
		fd = open(path, O_RDONLY);
	else
		fd = fi->fh;
	
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	if(fi == NULL)
		close(fd);
	return res;
}

static int nbfs_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;

	(void) fi;
	if(fi == NULL)
		fd = open(path, O_WRONLY);
	else
		fd = fi->fh;
	
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	if(fi == NULL)
		close(fd);
	return res;
}

static int nbfs_statfs(const char *path, struct statvfs *stbuf)
{
	int res;

	res = statvfs(path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int nbfs_release(const char *path, struct fuse_file_info *fi)
{
	(void) path;
	close(fi->fh);
	return 0;
}

static int nbfs_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int nbfs_fallocate(const char *path, int mode,
			off_t offset, off_t length, struct fuse_file_info *fi)
{
	int fd;
	int res;

	(void) fi;

	if (mode)
		return -EOPNOTSUPP;

	if(fi == NULL)
		fd = open(path, O_WRONLY);
	else
		fd = fi->fh;
	
	if (fd == -1)
		return -errno;

	res = -posix_fallocate(fd, offset, length);

	if(fi == NULL)
		close(fd);
	return res;
}
#endif

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int nbfs_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	int res = lsetxattr(path, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int nbfs_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	int res = lgetxattr(path, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int nbfs_listxattr(const char *path, char *list, size_t size)
{
	int res = llistxattr(path, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int nbfs_removexattr(const char *path, const char *name)
{
	int res = lremovexattr(path, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

#ifdef HAVE_COPY_FILE_RANGE
static ssize_t nbfs_copy_file_range(const char *path_in,
				   struct fuse_file_info *fi_in,
				   off_t offset_in, const char *path_out,
				   struct fuse_file_info *fi_out,
				   off_t offset_out, size_t len, int flags)
{
	int fd_in, fd_out;
	ssize_t res;

	if(fi_in == NULL)
		fd_in = open(path_in, O_RDONLY);
	else
		fd_in = fi_in->fh;

	if (fd_in == -1)
		return -errno;

	if(fi_out == NULL)
		fd_out = open(path_out, O_WRONLY);
	else
		fd_out = fi_out->fh;

	if (fd_out == -1) {
		close(fd_in);
		return -errno;
	}

	res = copy_file_range(fd_in, &offset_in, fd_out, &offset_out, len,
			      flags);
	if (res == -1)
		res = -errno;

	close(fd_in);
	close(fd_out);

	return res;
}
#endif

static off_t nbfs_lseek(const char *path, off_t off, int whence, struct fuse_file_info *fi)
{
	int fd;
	off_t res;

	if (fi == NULL)
		fd = open(path, O_RDONLY);
	else
		fd = fi->fh;

	if (fd == -1)
		return -errno;

	res = lseek(fd, off, whence);
	if (res == -1)
		res = -errno;

	if (fi == NULL)
		close(fd);
	return res;
}

static const struct fuse_operations nbfs_oper = {
//	.init           = nbfs_init,    // initiate the program 
	.getattr	= nbfs_getattr,   // OK
//	.access		= nbfs_access,    
	.readlink	= nbfs_readlink,  // 
	.readdir	= nbfs_readdir,   // 
//	.mknod		= nbfs_mknod,
	.mkdir		= nbfs_mkdir,     // OK
//	.symlink	= nbfs_symlink,
	.unlink		= nbfs_unlink,    // OK 
	.rmdir		= nbfs_rmdir,     // OK
	.rename		= nbfs_rename,    //
//	.link		= nbfs_link,
//	.chmod		= nbfs_chmod,
//	.chown		= nbfs_chown,
///	.truncate	= nbfs_truncate,
#ifdef HAVE_UTIMENSAT
//	.utimens	= nbfs_utimens,
#endif
	.open		= nbfs_open,
	.create 	= nbfs_create,
	.read		= nbfs_read,
	.write		= nbfs_write,
//	.statfs		= nbfs_statfs,
//	.release	= nbfs_release,
//	.fsync		= nbfs_fsync,
#ifdef HAVE_POSIX_FALLOCATE
//	.fallocate	= nbfs_fallocate,
#endif
#ifdef HAVE_SETXATTR
//	.setxattr	= nbfs_setxattr,
//	.getxattr	= nbfs_getxattr,
//	.listxattr	= nbfs_listxattr,
//	.removexattr	= nbfs_removexattr,
#endif
#ifdef HAVE_COPY_FILE_RANGE
//	.copy_file_range = nbfs_copy_file_range,
#endif
//	.lseek		= nbfs_lseek,
};
   
int main(int argc, char *argv[])
{
	umask(0);  //给用户最大权限  已录入问题解决 
	return fuse_main(argc, argv, &nbfs_oper, NULL);
}
