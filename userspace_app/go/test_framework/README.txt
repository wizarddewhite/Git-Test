1. Possible frameworks

https://learnku.com/articles/52896

    GoConvey: https://github.com/smartystreets/goconvey
    GoMock: https://github.com/golang/mock/
    GoMonkey: https://github.com/agiledragon/gomonkey
    GoMocket: https://github.com/Selvatico/go-mocket

1. Raw Test

$ cd raw_test
$ go test -v
$ go test -v -run TestAdd

Check test coverage
$ go test -cover

2. GoConvey

$ cd goconvey
$ go test -v
$ go test -v -run TestCheckUrl

3. GoMock(Not working...)

$ cd gomock

$ mockgen -source=./db/db.go -destination=./db/db_mock.go -package=main
