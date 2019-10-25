//
// Created by Yuxin Fan on 2019-10-14.
//

#include "ServerImpV2.h"

ServerImp::ServerImp(string path) {
    Service();
    this->root = path;
    this->pathToFh[""] = 0;
    this->fhToPath[0] = "";
    fhcount = 1;
}

Status ServerImp::call(ServerContext *context, const request *request,
                       response *response) {

    if (strcmp((request->command()).c_str(), "lookup") == 0) {
        cout << "--------lookup request--------" << endl;
        cout << request->dirfh() << endl;
        cout << request->name() << endl;
        string parent_path = this->fhToPath[request->dirfh()];
        cout << "parent path: " << parent_path << endl;
        string new_parent_path = parent_path + "/" + request->name();

        cout << "server lookup for " << new_parent_path << "\n";

        if (this->pathToFh.find(new_parent_path) == this->pathToFh.end()) {
            this->addLookup(new_parent_path);
        }
        cout << "server response to lookup" << this->pathToFh.find(new_parent_path)->second << "\n";
        response->set_fh(this->pathToFh.find(new_parent_path)->second);

    } else if (strcmp((request->command()).c_str(), "mkdir") == 0) {
        cout << "--------mkdir request--------" << endl;
        string newDir = this->root + this->findLookup(request->dirfh()) + "/" + request->name();
        mode_t mode = request->attr().st_mode();
        char *newDirChars;
        newDirChars = (char *) malloc(newDir.length() + 1);
        strcpy(newDirChars, newDir.c_str());

        if (mkdir(newDirChars, mode) != 0) {
            response->set_success(-1);
            response->set_ern(errno);
        } else {
            this->addLookup(this->findLookup(request->dirfh()) + "/" + request->name());
            response->set_success(0);
            response->set_ern(0);
        }
    } else if (strcmp((request->command()).c_str(), "rmdir") == 0) {
        cout << "--------rmdir request--------" << endl;
        string dirToRemove = this->root + this->findLookup(request->dirfh());
        char *dirToRemoveChars;
        dirToRemoveChars = (char *) malloc(dirToRemove.length() + 1);
        strcpy(dirToRemoveChars, dirToRemove.c_str());

        if (rmdir(dirToRemoveChars) != 0) {
            response->set_success(-1);
            response->set_ern(errno);
        } else {
            response->set_success(0);
            response->set_ern(0);
        }
        this->logLookup();
    } else if (strcmp((request->command()).c_str(), "rename") == 0) {
        cout << "--------rename request--------" << endl;

        string toDir = this->root + this->findLookup(request->todirfh()) + "/" + request->name();
        char *toDirChars;
        toDirChars = (char *) malloc(toDir.length() + 1);
        strcpy(toDirChars, toDir.c_str());

        string fromDir = this->root + this->findLookup(request->fromfh());
        char *fromDirChars;
        fromDirChars = (char *) malloc(fromDir.length() + 1);
        strcpy(fromDirChars, fromDir.c_str());

        if (rename(fromDirChars, toDirChars) != 0) {
            response->set_success(-1);
            response->set_ern(errno);
        } else {
            if (this->pathToFh.find(request->todirfh() + "/" + request->name()) != this->pathToFh.end()) {

                int afh = request->fromfh();
                int bfh = this->pathToFh[request->todirfh() + "/" + request->name()];

                this->pathToFh.erase(this->pathToFh.find(this->findLookup(afh)));
                this->fhToPath.erase(this->fhToPath.find(bfh));
                this->fhToPath[afh] = request->todirfh() + "/" + request->name();
                this->pathToFh[request->todirfh() + "/" + request->name()] = afh;
            } else {
                this->addLookup(this->findLookup(request->todirfh()) + "/" + request->name());
                this->pathToFh.erase(this->pathToFh.find(this->findLookup(request->fromfh())));
                this->fhToPath.erase(this->fhToPath.find(request->fromfh()));
            }

            response->set_success(0);
            response->set_ern(0);
        }
    } else if (strcmp((request->command()).c_str(), "open") == 0) {
        cout << "--------open request--------" << endl;

        string filepath = this->root + this->findLookup(request->fh());
        char *filepathChars;
        filepathChars = (char *) malloc(filepath.length() + 1);
        strcpy(filepathChars, filepath.c_str());

        struct fuse_file_info fi;
        toFuseFileInfo(request->fi(), &fi);

        int fh = open(filepathChars, fi.flags);
        cout << "open: " << fh << ", " << filepath << endl;

        if (fh == -1) {
            response->set_success(-1);
            response->set_ern(errno);
        } else {
            response->set_success(0);
            response->set_ern(errno);
        }
        fi.fh = fh;
        response->mutable_fi()->CopyFrom(toProtoFileInfo(&fi));
    } else if (strcmp((request->command()).c_str(), "create") == 0) {
        cout << "--------create request--------" << endl;

        string filepath = this->root + this->findLookup(request->dirfh()) + "/" + request->name();
        char *filepathChars;
        filepathChars = (char *) malloc(filepath.length() + 1);
        strcpy(filepathChars, filepath.c_str());
        mode_t mode = request->attr().st_mode();

        struct fuse_file_info fi;
        toFuseFileInfo(request->fi(), &fi);

        int fh = open(filepathChars, fi.flags, mode);
        cout << "create: " << fh << ", " << filepath << endl;

        if (fh == -1) {
            response->set_success(-1);
            response->set_ern(errno);
        } else {
            this->addLookup(this->findLookup(request->dirfh()) + "/" + request->name());
            response->set_success(0);
            response->set_ern(errno);
        }
        fi.fh = fh;
        response->mutable_fi()->CopyFrom(toProtoFileInfo(&fi));
    } else if (strcmp((request->command()).c_str(), "unlink") == 0) {

        std::cout << "--------server_unlink request--------" << std::endl;

        std::string filepath = this->root + this->findLookup(request->fh());
        char *filepathChars;
        filepathChars = (char *) malloc(filepath.length() + 1);
        strcpy(filepathChars, filepath.c_str());

        if (unlink(filepathChars) == -1) {
            response->set_success(-1);
            response->set_ern(errno);
        } else {
            response->set_success(0);
            response->set_ern(0);
        }
    } else if (strcmp((request->command()).c_str(), "readdir") == 0) {
        cout << "--------read directory request--------" << endl;

        DIR *dp;
        struct dirent *de;
        string qpath;

        string adjustedPath = this->root + this->findLookup(request->dirfh());
        char *path = new char[adjustedPath.length() + 1];
        strcpy(path, adjustedPath.c_str());

        cout << "server readdir\n";
        cout << "request path: " << request->path() << "\n";

        dp = opendir(path);
        if (dp == NULL) {
            response->set_status(-errno);
        } else {
            while ((de = readdir(dp)) != NULL) {

                struct stat st;
                directory *rd = response->add_objects();
                st.st_ino = de->d_ino;
                st.st_mode = de->d_type << 12;
                rd->set_name(de->d_name);
                *rd->mutable_attr() = toGrpcStat(&st);

                if (this->findLookup(request->dirfh())[this->findLookup(request->dirfh()).length() - 1] == '/')
                    qpath = this->findLookup(request->dirfh()) + rd->name();
                else
                    qpath = this->findLookup(request->dirfh()) + "/" + rd->name();

                this->addLookup(qpath);

            }
            response->set_status(0);
        }

        closedir(dp);
    } else if (strcmp((request->command()).c_str(), "getattr") == 0) {
        cout << "--------get attributes request--------" << endl;

        struct stat st;
        string filepath;
        filepath = request->path();

        toCmdStat(request->attr(), &st);

        string gotfromlookup = this->findLookup(request->fh());
        if (gotfromlookup.empty()) {
            this->addLookup(filepath);
        }
        gotfromlookup = this->findLookup(request->fh());

        string adjustedPath = this->root + gotfromlookup;

        char *path = new char[adjustedPath.length() + 1];
        strcpy(path, adjustedPath.c_str());
        cout << "request path " << path << endl;
        int res = lstat(path, &st);
        if (res == -1) {
            cout << "GetAttributes : Error getting stat -  " << errno << " Error message - " << strerror(errno) << "\n";
            response->set_status(-errno);
        } else {
            response->set_status(0);
            *response->mutable_attr() = toGrpcStat(&st);
        }

    } else if (strcmp((request->command()).c_str(), "read") == 0) {
        cout << "--------read request--------" << endl;
        string filepath = this->root + this->findLookup(request->fh());
        char *filepathChars;
        filepathChars = (char *) malloc(filepath.length() + 1);
        strcpy(filepathChars, filepath.c_str());

        char *buffer = new char[request->size()];

        struct fuse_file_info fi;
        toFuseFileInfo(request->fi(), &fi);

        int fileHandle = fi.fh;
        int op;

        if (fi.fh == 0) {
            fileHandle = open(filepathChars, O_RDONLY);
            fi.fh = fileHandle;
        }
        if (fileHandle == -1) {
            response->set_success(-1);
            response->set_ern(errno);
        } else {
            op = pread(fileHandle, buffer, request->size(), request->offset());
            cout<< "buffer: "<< buffer<< endl;
            if (op == -1) {
                response->set_success(-1);
                response->set_ern(errno);
            } else {
                response->set_data(buffer);
                response->set_size(op);
                response->mutable_fi()->CopyFrom(toProtoFileInfo(&fi));
            }
        }
        delete buffer;
    } else if (strcmp((request->command()).c_str(), "mknod") == 0) {

        string filepath = this->root + request->name();
        char *filepathChars;
        filepathChars = (char *) malloc(filepath.length() + 1);
        strcpy(filepathChars, filepath.c_str());

        int op;

        mode_t mode = request->attr().st_mode();
        dev_t rdev = request->attr().st_dev();

        if (S_ISFIFO(mode)) {
            op = mkfifo(filepathChars, mode);
        } else {
            op = mknod(filepathChars, mode, rdev);
        }

        if (op == -1) {
            response->set_success(-1);
            response->set_ern(errno);
        } else {
            response->set_success(0);
            response->set_ern(0);
        }
    } else if (strcmp((request->command()).c_str(), "release") == 0) {
        cout << "--------release request--------" << endl;

        string filepath = this->root + this->findLookup(request->fh());
        char *filepathChars;
        filepathChars = (char *) malloc(filepath.length() + 1);
        strcpy(filepathChars, filepath.c_str());

        struct fuse_file_info fi;
        toFuseFileInfo(request->fi(), &fi);

        (void) filepathChars;
        close(fi.fh);

        response->set_success(0);
        response->set_ern(0);
        response->mutable_fi()->CopyFrom(toProtoFileInfo(&fi));
    } else if (strcmp((request->command()).c_str(), "write") == 0) {
        cout << "--------write request--------" << endl;

        cout << "server write " << this->findLookup(request->fh()) << "\n";

        Write write;
        write.size = request->size();
        write.offset = request->offset();
        write.fh = request->fh();
        write.gen = request->gen();
        struct fuse_file_info fi;
        toFuseFileInfo(request->fi(), &fi);
        write.fi = toProtoFileInfo(&fi);
        write.data = request->data();

        cache.push_back(write);

        response->set_status(0);
        response->set_datasize(request->size());
        response->mutable_fi()->CopyFrom(write.fi);
    } else if (strcmp((request->command()).c_str(), "commit") == 0) {
        cout << "--------commit request--------" << endl;

        this->logSave();

        int fh, op;
        string pth;

        int request_fh=request->fh();
        cout << "trying to commit. " <<request->fh()<< "\n";

        int ready_to_write = true;
        for (int i = 0; i < request->gens_size(); i++) {
            int current_gen = request->gens(i);
            int found= false;
            for (auto a = this->cache.begin(); a != this->cache.end();a++) {
                if ((*a).gen == current_gen) {
                    found=true;
                    break;
                }
            }
            if (!found){
                ready_to_write=false;
            }
        }


        this->logSave();
        if (ready_to_write){
            response->set_success(0);
            for (int i = 0; i < request->gens_size(); i++) {
                int current_gen = request->gens(i);
                for (auto a = this->cache.begin(); a != this->cache.end();) {
                    if ((*a).gen == current_gen) {
                        fh = (*a).fi.fh();
                        if (fh == 0) {
                            pth = this->root + this->findLookup(request->fh());
                            fh = open(pth.c_str(), O_WRONLY);
                        }
                        op = pwrite(fh, (*a).data.c_str(), (*a).size, (*a).offset);
                        cout << "result of write, " << op << ", " << (*a).size << ", " << (*a).offset << "\n";
                        if (op == -1) {
                            response->set_success(-1);
                            return Status::OK;
                        }
                        a = this->cache.erase(a);
                        break;
                    }else{
                        a++;
                    }
                }
            }
        }else {
            // no entries found to write to the disk.
            response->set_success(1);
        }
    } else if (strcmp((request->command()).c_str(), "fsync") == 0) {
        cout << "--------fsync request--------" << endl;

        string adjustedPath = this->root + request->path();
        char *path = new char[adjustedPath.length() + 1];
        strcpy(path, adjustedPath.c_str());

        int isdatasync = request->isdatasync();
        struct fuse_file_info fi;
        toCmdFileInfo(request->fi(), &fi);

        (void) path;
        (void) isdatasync;
        (void) fi;
        response->set_status(0);
        *response->mutable_fi() = toGRPCFileInfo(&fi);
    } else if (strcmp((request->command()).c_str(), "flush") == 0) {
        cout << "--------flush request--------" << endl;

        string adjustedPath = this->root + request->path();
        char *path = new char[adjustedPath.length() + 1];
        strcpy(path, adjustedPath.c_str());

        struct fuse_file_info fi;
        toCmdFileInfo(request->fi(), &fi);

        (void) path;
        int res = close(dup(fi.fh));
        response->set_status(0);
        if (res == -1) {
            response->set_status(-errno);
        }
        *response->mutable_fi() = toGRPCFileInfo(&fi);
    }

    return Status::OK;
}

void ServerImp::logSave() {
    cout << "---\n";
    for (auto &i : this->cache) {
        cout << i.fh << " @ " << this->findLookup(i.fh) << ", " << i.size << ", " << i.offset << "\n";
    }
    cout << "---\n";
}


void ServerImp::logLookup() {
    cout << "---\n";
    for (auto &e : this->pathToFh) {
        cout << e.first << " : " << e.second << "\n";
    }
    cout << "---\n";
}

int ServerImp::addLookup(string path) {
    if (this->pathToFh.find(path) == this->pathToFh.end()) {
        this->pathToFh[path] = this->fhcount;
        this->fhToPath[this->fhcount] = path;
        this->fhcount++;
        cout << "added " << path << " at " << (this->fhcount - 1) << " in lookup \n";
        this->storeMapping();
    }
    return this->pathToFh[path];
}

string ServerImp::findLookup(int fh) {
    if (this->fhToPath.find(fh) == this->fhToPath.end()) {
        this->getMapping();
        if (this->fhToPath.find(fh) == this->fhToPath.end()) {
            return NULL;
        }
    }
    return this->fhToPath[fh];

}

void ServerImp::storeMapping() {
    convertPthFhToJson(this->pathToFh, "/fuse-nfs/log/pathToFh.json");
    convertFhPthToJson(this->fhToPath, "/fuse-nfs/log/fhToPath.json");
}


void ServerImp::getMapping() {
    this->pathToFh = convertJsonToPthFh("/fuse-nfs/log/pathToFh.json");
    this->fhToPath = convertJsonToFhPth("/fuse-nfs/log/fhToPath.json");
}





