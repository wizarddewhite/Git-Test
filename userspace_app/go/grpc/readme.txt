1. Reference

Document:
https://tutorialedge.net/golang/go-grpc-beginners-tutorial/
Code:
https://github.com/TutorialEdge/go-grpc-tutorial/

2. Build

2.1 Generate code from .proto

protoc --go_out=plugins=grpc:chat chat.proto

2.2 Build executable

go build client.go
go build server.go
