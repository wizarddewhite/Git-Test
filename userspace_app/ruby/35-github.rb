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


# get my commit info
#response = JSON.parse(RestClient.get('https://api.github.com/repos/'+ 'bitcoin/bitcoin' + '/commits',
#                {
#                  :params => {
#                    :access_token => 'Token',
#                    :since => Time.mktime(2017, 9, 15).iso8601
#                  }
#                }
#              ))
#puts response[0]["commit"]["committer"]["date"]

# get my commit static
response = JSON.parse(RestClient.get('https://api.github.com/repos/'+ 'bitcoin/bitcoin' + '/stats/participation',
                {
                  :params => {
                    #:access_token => 'Token',
                  }
                }
              ))
weekly_commits = response['all'].join(",")
puts weekly_commits
puts weekly_commits.split(",")[51]

# get my contributor info
#page = 1
#contributors = 0
#while true do
#  raw_response = RestClient.get('https://api.github.com/repos/'+ 'bitcoin/bitcoin' + '/contributors',
#                  {
#                    :params => {
#                      :access_token => 'token',
#                      :page => page
#                    }
#                  }
#                )
#  response = JSON.parse(raw_response)
#  puts "Page " + page.to_s() + " Number " + response.length.to_s()
#  contributors += response.length
#  if response.length == 0
#    break
#  end
#  page += 1
#end
#puts contributors


# get my watch/star/fork info
#response = JSON.parse(RestClient.get('https://api.github.com/repos/'+ 'bitcoin/bitcoin',
#                {
#                  :params => {
#                    :access_token => 'Token',
#                  }
#                }
#              ))
#puts response['stargazers_count']
#puts response['subscribers_count']
#puts response['forks_count']
