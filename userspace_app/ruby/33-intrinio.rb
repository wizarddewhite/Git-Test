require "http"
require "json"

username = "d2fd14cf52e3bef7c7fc7c6a763f5ba3"
password = "c17ac2a00ccd3731f407a42ea86978be"
request_url = "https://api.intrinio.com/companies?ticker=BABA"

response = HTTP.basic_auth(:user => username, :pass => password)
               .get(request_url)
               .body

company = JSON.parse(response)
puts company["ticker"]
