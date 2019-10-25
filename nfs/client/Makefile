PROTO_PATH = ../proto/

all:
	g++ -std=c++11 -Wall  `pkg-config --cflags --libs fuse3` -c Client.cpp -o Client.o
	g++ -std=c++11 `pkg-config --cflags --libs fuse3 protobuf grpc` -c -o NFSClient.o NFSClient.cpp
	g++ -std=c++11 $(PROTO_PATH)nfs.pb.o $(PROTO_PATH)nfs.grpc.pb.o Client.o NFSClient.o -L/usr/local/lib -lstdc++fs `pkg-config --libs --cflags fuse3 protobuf grpc++` -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl -o Client

clean:
	rm Client Client.o NFSClient.o