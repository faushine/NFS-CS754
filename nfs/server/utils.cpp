//
// Created by Yuxin Fan on 2019-10-14.
//

#include "utils.h"

string getCurrentDateTime(string s) {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    if (s == "now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if (s == "date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return string(buf);
}

void Logger(string host, string logMsg) {
    string filePath = "/nfslog/" + host + "/log_" + getCurrentDateTime("date") + ".txt";
    string now = getCurrentDateTime("now");
    ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
    ofs << now << '\t' << logMsg << '\n';
    ofs.close();
}

std::string toString(const char *path) {
    std::string a(path);
    return a;
}

fileInfo toProtoFileInfo(struct fuse_file_info *fi) {
    fileInfo pfi;
    pfi.set_fh(fi->fh);
    pfi.set_flags(fi->flags);
    return pfi;
}

void toFuseFileInfo(fileInfo pfi, struct fuse_file_info *fi) {
    fi->fh = pfi.fh();
    fi->flags = pfi.flags();
}

attributes toGrpcStat(struct stat *st) {
    attributes gstat;
    gstat.set_st_dev(st->st_dev);
    gstat.set_st_ino(st->st_ino);
    gstat.set_st_mode(st->st_mode);
    gstat.set_st_nlink(st->st_nlink);
    gstat.set_st_uid(st->st_uid);
    gstat.set_st_gid(st->st_gid);
    gstat.set_st_rdev(st->st_rdev);
    gstat.set_st_size(st->st_size);
    gstat.set_st_blksize(st->st_blksize);
    gstat.set_st_blocks(st->st_blocks);
    return gstat;
}

void toCmdStat(attributes gstat, struct stat *st) {

    st->st_dev = gstat.st_dev();
    st->st_ino = gstat.st_ino();
    st->st_mode = gstat.st_mode();
    st->st_nlink = gstat.st_nlink();
    st->st_uid = gstat.st_uid();
    st->st_gid = gstat.st_gid();
    st->st_rdev = gstat.st_rdev();
    st->st_size = gstat.st_size();
    st->st_blksize = gstat.st_blksize();
    st->st_blocks = gstat.st_blocks();
}

void toCmdFileInfo(fileInfo fuseFileInfo, struct fuse_file_info *fi) {
    fi->fh = fuseFileInfo.fh();
    fi->flags = fuseFileInfo.flags();
}


fileInfo toGRPCFileInfo(struct fuse_file_info *fi) {
    fileInfo fuseFileInfo;
    fuseFileInfo.set_fh(fi->fh);
    fuseFileInfo.set_flags(fi->flags);
    return fuseFileInfo;
}


void convertPthFhToJson(map<string,int> &map, string path) {
    json j_map(map);

    ofstream file(path);

    if (file.is_open()) {
        file << j_map;
        file.close();
    } else
        cout << "Unable to open file";
}

void convertFhPthToJson(map<int,string> &map, string path) {
    json j_map(map);

    ofstream file(path);

    if (file.is_open()) {
        file << j_map;
        file.close();
    } else
        cout << "Unable to open file";
}

map<int, string> convertJsonToFhPth(string path) {
    json j_map;
    ifstream file(path);

    if (file.is_open()) {
        file>>j_map;
        file.close();
    } else
        cerr << "Unable to open file";

    map<int, string> m_new = j_map;
    return m_new;
}

map<string, int> convertJsonToPthFh(string path) {
    json j_map;
    ifstream file(path);

    if (file.is_open()) {
        file>>j_map;
        file.close();
    } else
        cerr << "Unable to open file";

    map<string, int> m_new = j_map;
    return m_new;
}