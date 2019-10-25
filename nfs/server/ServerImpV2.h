//
// Created by Yuxin Fan on 2019-10-14.
//
#include <sys/stat.h>
#include <unistd.h>
#include <experimental/filesystem>
#include <sys/stat.h>
#include <dirent.h>
#include <map>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include "../proto/nfs.grpc.pb.h"
#include "utils.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using NFS::request;
using NFS::response;
using NFS::nfsServer;
using NFS::directory;
using namespace std;

class ServerImp final : public nfsServer::Service {
private:
    string root;
    map<string, int> pathToFh;
    map<int, string> fhToPath;
    int fhcount;
    vector<Write> cache;

    int addLookup(string path);
    string findLookup(int fh);
    void logLookup();
    void logSave();
    void storeMapping();
    void getMapping();

public:
    ServerImp(string path);

    Status call(ServerContext *context, const request *request, response *response) override;

};