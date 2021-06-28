1. Refer to 

https://echorand.me/posts/getting-started-with-golang-plugins/

2. Build Step

$ cd plugin1 && go build -buildmode=plugin && cd ..
$ cd plugin2 && go build -buildmode=plugin && cd ..
$ go run main.go
