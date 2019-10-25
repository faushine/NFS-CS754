all :
	protoc --cpp_out=. nfs.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` nfs.proto
	g++ -std=c++11 `pkg-config --cflags protobuf grpc` -c -o nfs.pb.o nfs.pb.cc
	g++ -std=c++11 `pkg-config --cflags protobuf grpc` -c -o nfs.grpc.pb.o nfs.grpc.pb.cc


clean :
	rm nfs.grpc.pb.cc nfs.grpc.pb.h
	rm nfs.pb.cc nfs.pb.h
	rm nfs.pb.o
	rm nfs.grpc.pb.o
