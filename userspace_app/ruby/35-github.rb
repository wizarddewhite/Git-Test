# http://wiki.jikexueyuan.com/project/github-developer-guides/getting-started.html
# https://www.gitbook.com/book/1030310877/github-api-v3/details

#require 'sinatra'
require 'rest-client'
require 'json'
require 'time'

# get my account info
#response = JSON.parse(RestClient.get('https://api.github.com/user',
#                {
#                  :params => {:access_token => 'Token'}
#                }
#              ))

#puts response['login']


# get my account info
response = JSON.parse(RestClient.get('https://api.github.com/repos/'+ 'bitcoin/bitcoin' + '/commits',
                {
                  :params => {
                    :access_token => 'Token',
                    :since => Time.mktime(2017, 9, 15).iso8601
                  }
                }
              ))

puts response[0]["commit"]["committer"]["date"]
