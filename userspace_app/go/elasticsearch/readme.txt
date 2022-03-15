Reference:
 * Concept https://mp.weixin.qq.com/s?__biz=MzkyNzI1NzM5NQ==&mid=2247484756&idx=1&sn=6029ed72391e175ebef3de47bf9290a2&source=41#wechat_redirect
 * Example https://segmentfault.com/a/1190000024438897

Code repo: https://github.com/asong2020/Golang_Dream.git
1. How to use this example

1.1 Build
git clone https://github.com/asong2020/Golang_Dream.git
cd Golang_Dream
cd code_demo/go-elastic-asong
go build
./go-elastic-asong

1.2 Create

$ curl --location -X POST 'http://localhost:8080/api/user/create' \
--header 'Content-Type: application/json' \
--data-raw '{
"id": 1,
"username": "asong1",
"nickname": "Golang梦工厂",
"phone": "17897875431",
"age": 21,
"ancestral": "吉林省深圳市",
"identity": "工人"
}'

$ curl --location -X POST 'http://localhost:8080/api/user/create' \
--header 'Content-Type: application/json' \
--data-raw '{
"id": 5,
"username": "asong5",
"nickname": "Golang梦工厂",
"phone": "17897875435",
"age": 25,
"ancestral": "吉林省深圳市",
"identity": "工人"
}'

$ curl --location -X POST 'http://localhost:8080/api/user/create' \
--header 'Content-Type: application/json' \
--data-raw '{
"id": 101,
"username": "asong101",
"nickname": "Golang梦工厂",
"phone": "17897875101",
"age": 101,
"ancestral": "吉林省长春市",
"identity": "工人"
}'

1.3 Update

$ curl --location --request PUT 'http://localhost:8080/api/user/update' \
--header 'Content-Type: application/json' \
--data-raw '{
"id": 1,
"username": "asong1",
"nickname": "Golang梦工厂",
"phone": "15088888888",
"age": 21,
"ancestral": "吉林省",
"identity": "工人"
}'

1.4 Delete

$ curl --location --request DELETE 'http://localhost:8080/api/user/delete' \
--header 'Content-Type: application/json' \
--data-raw '{
"id": 1,
"username": "asong1",
"nickname": "Golang梦工厂",
"phone": "15088888888",
"age": 21,
"ancestral": "吉林省",
"identity": "工人"
}'

1.5 Query

$ curl --location --request POST 'http://localhost:8080/api/user/search' \
--header 'Content-Type: application/json' \
--data-raw '{
    "nickname": "",
    "phone": "",
    "identity": "",
    "ancestral": "吉林省",
    "num": 1,
    "size":10
}'

$ curl --location --request GET 'http://localhost:8080/api/user/info?id=1,2,3'

2. Use Curl to access es directly
https://www.bbsmax.com/A/ke5jR8ma5r/

2.1 Display all data in index

curl --user user:passwd http://localhost:9200/asong_golang_dream/_search?pretty=true

2.2 Display based on id

curl --user user:passwd http://localhost:9200/asong_golang_dream/_doc/1?pretty=true

2.3 Delete record

curl --user user:passwd -X DELETE http://localhost:9200/asong_golang_dream/_doc/1

2.4 Delete index

curl --user user:passwd -X DELETE http://localhost:9200/asong_golang_dream

2.5 Query
https://www.tizi365.com/archives/628.html
