//
// Created by Yuxin Fan on 2019-10-14.
//

#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <fstream>
#include "../proto/nfs.grpc.pb.h"
#include "../proto/nfs.pb.h"
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <fstream>
using json = nlohmann::json;
using namespace std;
using NFS::fileInfo;
using NFS::attributes;

class Write {
public :
    int fh;
    int size;
    int offset;
    int gen;
    std::string data;
    fileInfo fi;
};

struct DirEntry {
    struct stat st;
    std::string name;
};

std::string toString(const char *path);

fileInfo toProtoFileInfo(struct fuse_file_info *fi);

void toFuseFileInfo(fileInfo pfi, struct fuse_file_info *fi);

attributes toGrpcStat(struct stat *st);

void toCmdStat(attributes gstat, struct stat *st);

void toCmdFileInfo(fileInfo fuseFileInfo, struct fuse_file_info *fi);

fileInfo toGRPCFileInfo(struct fuse_file_info *fi);

string getCurrentDateTime(string s);

void Logger(string host, string logMsg);

map<int, string> convertJsonToFhPth(string path);

map<string, int> convertJsonToPthFh(string path);

void convertPthFhToJson(map<string, int> &map, string path);

void convertFhPthToJson(map<int, string> &map, string path);
