//
// Created by l367zhan on 2019-10-14.
//

#include "NFSClient.h"
#include <chrono>
#include <thread>
#include <unistd.h>
#include <stdlib.h>

#define LOG true

ClientImp::ClientImp(std::shared_ptr <Channel> channel)
        : stub_(nfsServer::NewStub(channel)) {
    this->reset_maps();
    this->renewed = true;
}


fileInfo ClientImp::fusetoProto(struct fuse_file_info *fi) {
    fileInfo pfi;
    pfi.set_fh(fi->fh);
    pfi.set_flags(fi->flags);
    return pfi;
}

void ClientImp::readFileInfo(fileInfo pfi, struct fuse_file_info *fi) {
    fi->fh = pfi.fh();
    fi->flags = pfi.flags();
}

attributes ClientImp::toGRPCstat(struct stat *st) {
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

void ClientImp::toCmdstat(attributes gstat, struct stat *st) {

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


void ClientImp::reconnect() {
    this->stub_ = nfsServer::NewStub(grpc::CreateChannel("localhost:3110", grpc::InsecureChannelCredentials()));
    this->renewed = true;
}

void ClientImp::retry() {
    this->renewed = false;
    std::cout << "Communication failed" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "retrying" << std::endl;
    if (!renewed) {
        this->reset_maps();
        this->reconnect();
    }
}

void ClientImp::reset_maps() {
    this->pathtofh.clear();
    this->fhtopath.clear();
    this->pathtofh[""] = 0;
    this->fhtopath[0] = "";
}

std::string get_parent_path(std::string pth) {
    path ref(pth);
    path parentPath = ref.parent_path();
    std::string parentPathString = parentPath.string();
    if (parentPathString.length() == 1) {
        parentPathString = "";
    }
    return parentPathString;
}


int ClientImp::lookup(std::string ref) {

    path refpath(ref);
    std::string refpathstring = refpath.string();
    if (refpathstring.length() == 1) {
        refpathstring = "";
    }

    if (this->pathtofh.find(refpathstring) == this->pathtofh.end()) {

        std::string parentpathstring = get_parent_path(refpathstring);
        this->lookup(parentpathstring);

        request request;
        response response;
        ClientContext context;

        request.set_command("lookup");
        request.set_dirfh(this->pathtofh[parentpathstring]);
        std::string last;
        for (auto &e : refpath)
            last = e.string();
        request.set_name(last);

        std::cout << "Sending request: lookup " << "\n";
        Status status = stub_->call(&context, request, &response);

        if (status.ok()) {
            this->pathtofh[refpathstring] = response.fh();
            this->fhtopath[response.fh()] = refpathstring;
            return this->pathtofh[refpathstring];
        } else {
            this->retry();
            return this->lookup(ref);
        }
    } else {
        return this->pathtofh[refpathstring];
    }
}


int ClientImp::client_commit(int fh) {

    ClientContext context;
    request request;
    response response;

    std::cout << "commit fh: " << fh << std::endl;
    request.set_command("commit");
    request.set_fh(fh);
    bool dirty = false;
    for (auto &chunk:this->cache) {
        if (fh == chunk.fh) {
            std::cout << "found: " << fh << std::endl;
            dirty = true;
            request.add_gens(chunk.gen);
            std::cout << "offset" << chunk.offset << std::endl;
        }
    }
    if (!dirty) {
        std::cout << "Don't need to commit. " << std::endl;
        return 0;
    }
    std::cout << "Sending request: commit " << std::endl;
    Status status = stub_->call(&context, request, &response);

    if (status.ok()) {
        if (response.success() == 0) {
            for (auto chunk = this->cache.begin(); chunk != this->cache.end();) {
                if (fh == (*chunk).fh) {
                    chunk = this->cache.erase(chunk);
                } else {
                    chunk++;
                }
            }
            return 0;
        } else if (response.success() == 1) {
            // server does not have all the copies
            std::cout << "Chunks resend: " << std::endl;
            this->client_write_resend();
            return this->client_commit(fh);
        } else {
            return -1;
        }
    } else {
        this->retry();
        return this->client_commit(fh);
    }
}

response ClientImp::client_write_handler(Chunk chunk) {

    ClientContext context;
    request request;
    response response;

    request.set_command("write");
    request.set_size(chunk.size);
    request.set_offset(chunk.offset);
    request.set_fh(chunk.fh);
    request.set_data(chunk.data);
    request.set_gen(chunk.gen);
    request.mutable_fi()->CopyFrom(chunk.fi);

    std::cout << "Sending request: write " << std::endl;
    Status status = stub_->call(&context, request, &response);

    if (status.ok()) {
        return response;
    } else {
        this->retry();
        return this->client_write_handler(chunk);
    }
}


void ClientImp::client_write_resend() {
    std::cout << "cache size " << this->cache.size() << std::endl;
    for (auto &chunk:this->cache) {
        std::cout << "Resending chunk " << chunk.gen << std::endl;
        response response = client_write_handler(chunk);
        while (response.status() != 0) {
            response = client_write_handler(chunk);
        }
    }
}

int ClientImp::client_mkdir(std::string pth, mode_t mode) {

    ClientContext context;
    request request;
    response response;
    attributes attr;

    path new_path = pth;
    int dirfh = this->lookup(get_parent_path(pth));
    std::string filename = new_path.filename();

    request.set_command("mkdir");
    request.set_dirfh(dirfh);
    request.set_name(filename);
    request.mutable_attr()->CopyFrom(attr);
    attr.set_st_mode(mode);

    std::cout << "Sending request: mkdir " << std::endl;
    Status status = stub_->call(&context, request, &response);

    if (status.ok()) {
        if (response.success() != 0) {
            return (-response.ern());
        }
        return 0;
    } else {
        this->retry();
        return this->client_mkdir(pth, mode);
    }
}

int ClientImp::client_rmdir(std::string pth) {

    ClientContext context;
    request request;
    response response;

    request.set_command("rmdir");
    request.set_dirfh(this->lookup(pth));

    std::cout << "Sending request: rmdir " << std::endl;
    Status status = stub_->call(&context, request, &response);

    if (status.ok()) {
        if (response.success() != 0) {
            return (-response.ern());
        }
        this->fhtopath.erase(this->lookup(pth));
        this->pathtofh.erase(pth);
        return 0;
    } else {
        this->retry();
        return this->client_rmdir(pth);
    }
}

int ClientImp::client_rename(std::string from, std::string to) {

    ClientContext context;
    request request;
    response response;

    path new_path = to;
    std::string filename = new_path.filename();

    request.set_command("rename");
    request.set_fromfh(this->lookup(from));
    request.set_todirfh(this->lookup(get_parent_path(to)));
    request.set_name(filename);

    std::cout << "Sending request: rename " << std::endl;
    Status status = stub_->call(&context, request, &response);

    if (status.ok()) {
        if (response.success() != 0) {
            return (-response.ern());
        }

        if (this->pathtofh.find(to) != this->pathtofh.end()) {
            //renaming to an existing file, case of overwrite

            int fromfh = this->pathtofh[from];
            int tofh = this->pathtofh[to];

            this->pathtofh.erase(this->pathtofh.find(from));
            this->fhtopath.erase(this->fhtopath.find(tofh));
            this->fhtopath[fromfh] = to;
            this->pathtofh[to] = fromfh;
        } else {
            this->fhtopath.erase(this->fhtopath.find(this->pathtofh[from]));
            this->pathtofh.erase(this->pathtofh.find(from));
        }

        return 0;
    } else {
        this->retry();
        return this->client_rename(from, to);
    }
}

int ClientImp::client_open(std::string pth, struct fuse_file_info *fi) {

    ClientContext context;
    request request;
    response response;

    request.set_command("open");
    request.set_fh(this->lookup(pth));
    request.mutable_fi()->CopyFrom(fusetoProto(fi));

    std::cout << "Sending request: open " << std::endl;
    Status status = stub_->call(&context, request, &response);

    if (status.ok()) {
        readFileInfo(response.fi(), fi);
        if (response.success() != 0) {
            return (-response.ern());
        }
        return 0;
    } else {
        this->retry();
        return this->client_open(pth, fi);
    }
}


int ClientImp::client_create(std::string pth, mode_t mode, struct fuse_file_info *fi) {

    ClientContext context;
    request request;
    response response;
    attributes attr;

    path new_path = pth;
    std::string filename = new_path.filename();

    request.set_command("create");
    request.set_dirfh(this->lookup(get_parent_path(pth)));
    request.set_name(filename);
    attr.set_st_mode(mode);
    request.mutable_attr()->CopyFrom(attr);
    request.mutable_fi()->CopyFrom(fusetoProto(fi));

    std::cout << "Sending request: create " << std::endl;
    Status status = stub_->call(&context, request, &response);

    if (status.ok()) {
        readFileInfo(response.fi(), fi);
        if (response.success() != 0) {
            return (-response.ern());
        }
        return 0;
    } else {
        this->retry();
        return this->client_create(pth, mode, fi);
    }
}

int ClientImp::client_read(std::string pth, char *buffer, int size, int offset, struct fuse_file_info *fi) {

    ClientContext context;
    request request;
    response response;

    request.set_command("read");
    request.set_fh(this->lookup(pth));
    request.set_size(size);
    request.set_offset(offset);
    request.mutable_fi()->CopyFrom(fusetoProto(fi));

    std::cout << "Sending request: read " << std::endl;
    Status status = stub_->call(&context, request, &response);


    if (status.ok()) {
        readFileInfo(response.fi(), fi);
        strncpy(buffer, response.data().c_str(), size);
        std::cout<< "buffer: "<< buffer<< std::endl;
        if (response.success() != 0) {
            return (-response.ern());
        }
        return response.size();
    } else {
        this->retry();
        return this->client_read(pth, buffer, size, offset, fi);
    }
}


std::list <DirEntry> ClientImp::read_directory(std::string pth, int &responseCode) {

    ClientContext context;
    request request;
    response response;

    request.set_command("readdir");
    request.set_dirfh(this->lookup(pth));

    std::cout << "Sending request: readdir " << std::endl;
    Status status = stub_->call(&context, request, &response);

    std::list <DirEntry> entries;
    if (status.ok()) {
        responseCode = response.status();
        for (int i = 0; i < response.objects_size(); i++) {
            DirEntry dirEntry;
            toCmdstat(response.objects(i).attr(), &dirEntry.st);
            dirEntry.name = response.objects(i).name();
            entries.push_back(dirEntry);
        }
        return entries;
    } else {
        this->retry();
        return this->read_directory(pth, responseCode);
    }
}

int ClientImp::client_getattr(std::string pth, struct stat *st) {

    ClientContext context;
    request request;
    response response;

    request.set_command("getattr");
    request.set_fh(this->lookup(pth));
    request.set_path(pth);
    *request.mutable_attr() = toGRPCstat(st);

    std::cout << "Sending request: getattr " << std::endl;
    Status status = stub_->call(&context, request, &response);

    if (status.ok()) {
        toCmdstat(response.attr(), st);
        std::cout << response.status() << std::endl;
        return response.status();
    } else {
        this->retry();
        return this->client_getattr(pth, st);
    }
}


int ClientImp::client_release(std::string pth, struct fuse_file_info *fi, bool useCommit) {

    if (useCommit) {
        int result = client_commit(this->lookup(pth));
        if (result == -1) {
            return result;
        }
    }

    ClientContext context;
    request request;
    response response;

    request.set_command("release");
    request.set_fh(this->lookup(pth));
    request.mutable_fi()->CopyFrom(fusetoProto(fi));

    std::cout << "Sending request: release " << std::endl;
    Status status = stub_->call(&context, request, &response);


    if (status.ok()) {
        readFileInfo(response.fi(), fi);
        if (response.success() != 0) {
            return (-response.ern());
        }
        return 0;
    } else {
        this->retry();
        return this->client_release(pth, fi, useCommit);
    }
}


int ClientImp::client_write(std::string pth, const char *buf, int size, int offset, struct fuse_file_info *fi) {

    ClientContext context;
    request request;
    response response;

    fileInfo pfi = fusetoProto(fi);
    std::string buffer(buf);
    int gen = rand() % 10000;

    Chunk chunk = Chunk(this->lookup(pth), size, offset, buffer, pfi, gen);
    std::cout << "client write offset: " << offset<<" chunk: "<<chunk.offset<<std::endl;
    this->cache.push_back(chunk);

    response = client_write_handler(chunk);

    if (response.status() == 0) {
        readFileInfo(response.fi(), fi);
        return response.datasize();
    }
    return -1;
}

int ClientImp::client_unlink(std::string pth) {

    ClientContext context;
    request request;
    response response;

    request.set_command("unlink");
    request.set_fh(this->lookup(pth));

    std::cout << "Sending request: unlink " << std::endl;
    Status status = stub_->call(&context, request, &response);

    if (status.ok()) {
        if (response.success() != 0) {
            return (-response.ern());
        }
        return 0;
    } else {
        this->retry();
        return this->client_unlink(pth);
    }
}


int ClientImp::client_fsync(std::string pth, int isDataSync, struct fuse_file_info *fi, bool useCommit) {


    ClientContext context;
    request request;
    response response;

    fileInfo pfi = fusetoProto(fi);

    request.set_command("fsync");
    request.set_path(pth);
    request.set_isdatasync(isDataSync);
    request.mutable_fi()->CopyFrom(pfi);

    std::cout << "Sending request: fsync " << std::endl;
    Status status = stub_->call(&context, request, &response);

    if (status.ok()) {
        return response.status();
    } else {
        this->retry();
        return this->client_fsync(pth, isDataSync, fi, useCommit);
    }
}

int ClientImp::client_flush(std::string pth, struct fuse_file_info *fi, bool useCommit) {

//    if (useCommit) {
//        client_commit(this->lookup(pth));
//    }

    ClientContext context;
    request request;
    response response;

    fileInfo pfi = fusetoProto(fi);

    request.set_command("flush");
    request.set_path(pth);
    request.mutable_fi()->CopyFrom(pfi);

    std::cout << "Sending request: flush " << std::endl;
    Status status = stub_->call(&context, request, &response);

    if (status.ok()) {
        return response.status();
    } else {
        this->retry();
        return this->client_flush(pth, fi, useCommit);
    }

}
