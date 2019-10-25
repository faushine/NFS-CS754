//
// Created by Yuxin Fan on 2019-10-14.
//
#include "ServerImpV2.h"

void server(){
    string hostname ("localhost:3110");
    ServerImp serverImp("/fuse-nfs/mount");
    ServerBuilder builder;
    builder.AddListeningPort(hostname, grpc::InsecureServerCredentials());
    builder.RegisterService(&serverImp);
    unique_ptr<Server> server(builder.BuildAndStart());
    cout << "Server listening on " << hostname << endl;
    server->Wait();
}

int main(int argc, char **argv){
    server();
    return 0;
}