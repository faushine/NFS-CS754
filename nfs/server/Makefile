PROTO_PATH = ../proto/

all :

	g++ -std=c++11 `pkg-config --cflags --libs fuse3 protobuf grpc` utils.cpp -c -o utils.o
	g++ -std=c++11 `pkg-config --cflags --libs fuse3 protobuf grpc` -c -o ServerImpV1.o ServerImpV1.cpp
	g++ -std=c++11 `pkg-config --cflags --libs fuse3 protobuf grpc` -c -o ServerV1.o ServerV1.cpp
	g++ -std=c++11 $(PROTO_PATH)nfs.pb.o $(PROTO_PATH)nfs.grpc.pb.o ServerImpV1.o ServerV1.o utils.o -L/usr/local/lib  -lstdc++fs `pkg-config --libs --cflags fuse3 protobuf grpc++` -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl -o ServerV1

	g++ -std=c++11 `pkg-config --cflags --libs fuse3 protobuf grpc` -c -o ServerImpV2.o ServerImpV2.cpp
	g++ -std=c++11 `pkg-config --cflags --libs fuse3 protobuf grpc` -c -o ServerV2.o ServerV2.cpp
	g++ -std=c++11 $(PROTO_PATH)nfs.pb.o $(PROTO_PATH)nfs.grpc.pb.o ServerImpV2.o ServerV2.o utils.o -L/usr/local/lib  -lstdc++fs `pkg-config --libs --cflags fuse3 protobuf grpc++` -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl -o ServerV2

clean:
	rm ServerImpV1.o ServerV1.o ServerV1
	rm ServerImpV2.o ServerV2.o ServerV2


