//
// Created by l367zhan on 2019-10-14.
//

#ifndef NFS_NFSCLIENT_H
#define NFS_NFSCLIENT_H

#define FUSE_USE_VERSION 31

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <experimental/filesystem>
#include <sys/stat.h>
#include <fuse.h>

#include "../proto/nfs.grpc.pb.h"
#include "../proto/nfs.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using std::experimental::filesystem::path;

using NFS::fileInfo;
using NFS::request;
using NFS::response;
using NFS::attributes;
using NFS::directory;
using NFS::nfsServer;


class Chunk {

public :
    int fh;
    int size;
    int offset;
    std::string data;
    fileInfo fi;
    int gen;

    Chunk(int fh, int size, int offset, std::string data, fileInfo fi, int gen) : fh(fh), size(size),offset(offset), data(data),
                                                                                  fi(fi), gen(gen) {}
};

struct DirEntry {
    struct stat st;
    std::string name;
};


class ClientImp {

private:
    std::unique_ptr <nfsServer::Stub> stub_;
    std::map<std::string, int> pathtofh;
    std::map<int, std::string> fhtopath;
    std::vector <Chunk> cache;
    bool renewed;

    int lookup(std::string pth);

    fileInfo fusetoProto(struct fuse_file_info *fi);

    void readFileInfo(fileInfo pfi, struct fuse_file_info *fi);

    attributes toGRPCstat(struct stat *st);

    void toCmdstat(attributes gstat, struct stat *st);

    int client_commit(int fh);

    void client_write_resend();

    response client_write_handler(Chunk chunk);

    void reset_maps();

    void reconnect();

    void retry();

public:
    ClientImp(std::shared_ptr <Channel> channel);

    int client_mkdir(std::string pth, mode_t mode);

    int client_rmdir(std::string path);

    int client_rename(std::string from, std::string to);

    int client_open(std::string pth, struct fuse_file_info *fi);

    int client_create(std::string pth, mode_t mode, struct fuse_file_info *fi);

    int client_read(std::string path, char *buffer, int size, int offset, struct fuse_file_info *fi);

    std::list <DirEntry> read_directory(std::string path, int &responseCode);

    int client_getattr(std::string pth, struct stat *st);

    int client_release(std::string pth, struct fuse_file_info *fi, bool useCommit);

    int client_write(std::string pth, const char *buf, int size, int offset, struct fuse_file_info *fi);

    int client_unlink(std::string path);

    int client_fsync(std::string pth, int isDataSync, struct fuse_file_info *fi, bool useCommit);

    int client_flush(std::string pth, struct fuse_file_info *fi, bool useCommit);

    void print_storage();

};


#endif //NFS_NFSCLIENT_H
