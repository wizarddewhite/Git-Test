extern crate micro_http;
use std::fs;
pub use micro_http::{
    Body, HttpServer, Method, Request, RequestError, Response, ServerError, ServerRequest,
    ServerResponse, StatusCode, Version,
};

fn main() {
    let mut server = HttpServer::new("/tmp/firecracker.socket".to_string()).unwrap();
    server.start_server().unwrap();
    println!("Hello, world!");

    // let mut req_vec = server.requests().unwrap();
    // let server_request = req_vec.remove(0);

    loop {

    match server.requests() {
        Ok(request_vec) => {
            println!("Get a request vec {}", request_vec.len());
            for server_request in request_vec {
                server.respond(
                    server_request.process( |request| {
                       let request_uri = request.uri().get_abs_path().to_string();
                       println!("uri {}", request_uri);
                       let path_tokens: Vec<&str> = request_uri[1..].split_terminator('/').collect();
                       println!("path_token {}", path_tokens[0]);


                       let mut response = Response::new(Version::Http11, StatusCode::OK);
                       let response_body = b"ywtest response body";
                       response.set_body(Body::new(response_body.to_vec()));
                       response
                    })
                ).unwrap();
            }
        }
        Err(e) => {
            println!("Error {}", e);
        }
    }

    }

    fs::remove_file("/tmp/firecracker.socket").unwrap();
}
