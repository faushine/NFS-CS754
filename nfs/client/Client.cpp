//
// Created by l367zhan on 2019-10-17.
//

#define FUSE_USE_VERSION 31

#include "fuse.h"
#include <sys/stat.h>
#include <fuse.h>
#include "NFSClient.h"
#include <stdio.h>

struct fuse_operations nfsOps;

std::string toStr(const char *path) {
    std::string a(path);
    return a;
}

bool useCommit = true;


static ClientImp NFSClient(grpc::CreateChannel("localhost:3110", grpc::InsecureChannelCredentials()));

int nfs_getattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi) {
    std::string pathstr(path);
    std::cout << path << std::endl;
    return NFSClient.client_getattr(pathstr, statbuf);
}

int nfs_mkdir(const char *path, mode_t mode) {
    return NFSClient.client_mkdir(toStr(path), mode);
}

int nfs_rmdir(const char *path) {
    return NFSClient.client_rmdir(toStr(path));
}

int nfs_rename(const char *path, const char *newpath, unsigned int flag) {
    return NFSClient.client_rename(toStr(path), toStr(newpath));
}

int nfs_create(const char *path, mode_t mode, struct fuse_file_info *fileInfo) {
    return NFSClient.client_create(toStr(path), mode, fileInfo);
}

int nfs_open(const char *path, struct fuse_file_info *fileInfo) {
    return NFSClient.client_open(toStr(path), fileInfo);
}

int nfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    return NFSClient.client_read(path, buf, size, offset, fileInfo);
}

int nfs_unlink(const char *path) {
    return NFSClient.client_unlink(path);
}

int nfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    std::string pathstr(path);
    return NFSClient.client_write(pathstr, buf, size, offset, fileInfo);
}

int nfs_flush(const char *path, struct fuse_file_info *fileInfo) {
    std::string pathstr(path);
    return NFSClient.client_flush(pathstr, fileInfo, useCommit);
}

int nfs_release(const char *path, struct fuse_file_info *fileInfo) {
    return NFSClient.client_release(path, fileInfo, useCommit);
}

int nfs_fsync(const char *path, int isDataSync, struct fuse_file_info *fileInfo) {
    return NFSClient.client_fsync(path, isDataSync, fileInfo, useCommit);
}

int nfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo,
                fuse_readdir_flags rflags) {
    int responseCode;
    std::string pathValue(path);
    std::list <DirEntry> dirEntries = NFSClient.read_directory(pathValue, responseCode);
    for (auto const &dirEntry : dirEntries) {
        filler(buf, dirEntry.name.c_str(), &dirEntry.st, 0, FUSE_FILL_DIR_PLUS);
    }
    return responseCode;
}


int main(int argc, char **argv) {
    nfsOps.getattr = &nfs_getattr;
    nfsOps.readdir = &nfs_readdir;
    nfsOps.read = &nfs_read;
    nfsOps.mkdir = &nfs_mkdir;
    nfsOps.rmdir = &nfs_rmdir;
    nfsOps.rename = &nfs_rename;
    nfsOps.create = &nfs_create;
    nfsOps.open = &nfs_open;
    nfsOps.release = &nfs_release;
    nfsOps.unlink = &nfs_unlink;
    nfsOps.write = &nfs_write;
    nfsOps.flush = &nfs_flush;
    nfsOps.fsync = &nfs_fsync;
    return fuse_main(argc, argv, &nfsOps, NULL);
}
