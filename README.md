# NFS-CS754
This project is a basic implementation of NFS-like file system that supports Linux file system operations. We have tried to stick to NFS-3.0 semantics.

## Usage

Requirements:
- JSON for Modern C++: 3.7.0
- fuse: 3.7.0
- protobuf-cpp: 3.10.0
- gRPC: 1.25.0

```
# compile 
cd nfs
make
```
```
# run server V1
cd server
./ServerV1    # or ./ServerV2
```
```
# run client
cd nfs/client
./Client -f /mount/point/path
```

## Client

The file system implements the Linux client side by using FUSE. FUSE allows you to make something that looks like a file system but actually is built in user space. All operations in command line will be redirected to FUSE if the function is implemented. Then it calls gRPC procedure to execute the real function in the server side.

## Server

The NFS protocol is stateless, meaning that there is no need to maintain information about the protocol on the server. When server recives the request, it passes parameters to the specific linux system call.  

The server side contains two versions: 
- V1 for basic implementation;
- V2 for optimization using COMMIT.

### Why COMMIT?

One problem with classic NFS v2 WRITE protocol requests is that they must be performed synchronously by the server. This makes each write quite slow form the client's perspective. A small optimization that we add here is a COMMIT protocol message(like NFS-3.0 way). COMMIT allows each WRITE to be ack'd asynchronously. The way this works is that each WRITE is sent by a client and immediately ack'd by the server; only when COMMIT is issued by the client do all the previous WRITEs get committed to disk, thus enabling them to be batched into a single larger I/O. Once the COMMIT is ack'd, the client knows it can release the copies it is keeping in case of retransmission. The client sends a COMMIT request only when the file is closed.

## Crash Consistency
We maintain a persistent copy of this map by force writing them to the disk(two JSON files in /log). Every time the server crashes and restarts, it recovers the entries from this persistent copy. 

## Future Works
We consider improving our server design by using a thread pool to handle requests. And mapping multiplexing requests onto some number of threads (reusing threads between requests). For each update, we assign a mutex for each file and make sure that one file is processed by one thread at a time.


