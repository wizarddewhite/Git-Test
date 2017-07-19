require 'net/http'
require 'net/http/post/multipart'
require 'json'

def post_video_to_wistia(name, path_to_video)
  uri = URI('https://upload.wistia.com/')
  puts "Try to upload file #{path_to_video}"
  http = Net::HTTP.new(uri.host, uri.port)
  http.use_ssl = true

  # Construct the request.
  request = Net::HTTP::Post::Multipart.new uri.request_uri, {
    'api_password' => 'password',
    #'contact_id'   => '<CONTACT_ID>', # Optional.
    #'project_id'   => '<PROJECT_ID>', # Optional.
    'name'         => name, # Optional.

    'file' => UploadIO.new(
                File.open(path_to_video),
                'application/octet-stream',
                File.basename(path_to_video)
              )
  }

  # Make it so!
  response = http.request(request)

  return response
end

resp = post_video_to_wistia("local_upload", "/Users/weiyang/git/railsbridge/programming-exercise/WeChatSight8.mp4")

puts resp.code
puts resp.body

res = JSON.parse(resp.body)
puts "hashed_id #{res["hashed_id"]}"
