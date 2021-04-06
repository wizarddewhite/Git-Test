1. Reference

Document:
https://tutorialedge.net/golang/go-grpc-beginners-tutorial/
Code:
https://github.com/TutorialEdge/go-grpc-tutorial/

2. How to write a grpc application

2.0 Prerequisites

Protocol Buffers v3 installed

$ go get -u github.com/golang/protobuf/protoc-gen-go

2.1 Write .proto file

For example this is chat.proto here.

2.2 Generate code from .proto

$ protoc --go_out=plugins=grpc:chat chat.proto

The output file is in chat/chat.go

2.3 Implement the method defined in .proto

File chat/chat.go implement method SayHello().

2.4 Server and Client code

Now utilize chat/chat.go in client.go and server.go.

2.5 Build executable

$ go build client.go
$ go build server.go
