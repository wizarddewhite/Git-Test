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

2.5.1 Filter by date
Time Format https://blog.csdn.net/weixin_30608131/article/details/98046992
2.5.1.1 Start From
POST /instance/_search
{
    "from" : 0,
    "size" : 0,
    "query": {
        "bool": {
            "must": [
                {
                  "range": {
                        "create_time": {
                          "gte": "2018-01-01 00:00:00", 
                          "format": "yyyy-MM-dd hh:mm:ss"
                        }
                    }
                }
            ]
        }
    }
}

2.5.1.2 Start From yesterday
POST /instance/_search
{
  "size" : 10,
    "query": {
        "bool": {
            "must": [
                {
                  "range": {
                        "create_time": {
                          "gte": "now-1d/d"
                        }
                    }
                }
            ]
        }
    }
}

2.6 Aggregation

2.6.1 agg sum

curl --user user:passwd -X POST http://localhost:9200/asong_golang_dream/_search?size=0\&pretty=true \
--header 'Content-Type: application/json' \
--data-raw '{
  "query": { // 设置query查询条件，后面的aggs统计，仅对query查询结果进行统计
    "constant_score": {
      "filter": {
        "match": {
          "instype": "abcd"
        }
      }
    }
  },
  "aggs": { // aggregation
    "sum_cpu" : { "sum" : { "field" : "capcpu" } }
  }
}'

2.6.2 agg instype

curl --user user:passwd -X POST http://localhost:9200/asong_golang_dream/_search?size=0\&pretty=true \
--header 'Content-Type: application/json' \
--data-raw '{
  "aggs": {
    "type_ins" : { "terms" : { "field" : "instype" } }
  }
}'

2.6.3 data histogram

curl --user user:passwd -X POST http://localhost:9200/asong_golang_dream/_search?size=0\&pretty=true \
--header 'Content-Type: application/json' \
--data-raw '{
    "aggs" : {
        "sales_over_time" : { // 聚合查询名字，随便取一个
            "date_histogram" : { // 聚合类型为: date_histogram
                "field" : "time", // 根据date字段分组
                "calendar_interval" : "day", // 分组间隔：month代表每月、支持minute（每分钟）、hour（每小时）、day（每天）、week（每周）、year（每年）
                "format" : "yyyy-MM-dd" // 设置返回结果中桶key的时间格式
            }
        }
    }
}'

2.6.4 agg on instype and get sum/max

curl --user user:passwd -X POST \
http://localhost:9200/asong_golang_dream/_search?size=0\&pretty=true \
--header 'Content-Type: application/json' \
--data-raw '{
    "aggs" : {
        "sales_over_time" : { // 聚合查询名字，随便取一个
            "date_histogram" : { // 聚合类型为: date_histogram
                "field" : "time", // 根据date字段分组
                "calendar_interval" : "day", // 分组间隔：month代表每月、支持minute（每分钟）、hour（每小时）、day（每天）、week（每周）、year（每年）
                "format" : "yyyy-MM-dd" // 设置返回结果中桶key的时间格式
            },
            "aggs" : {
                "type_ins" : {
                    "terms" : {
                        "field" : "instype"
                    },
                    "aggs": {
                        "total_cpu" : { "sum" : { "field" : "capcpu" } },
                        "max_cpu" : { "max" : { "field" : "capcpu" } }
                    }
                }
            }
        }
    }
}'
