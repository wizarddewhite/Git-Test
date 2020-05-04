extern crate clap;

use clap::{Arg, App};

fn main() {
    let matches = App::new("MayApp")
        .version("0.1")
        .author("weiyang")
        .about("Learn use Rust Crate!")
        .arg(Arg::with_name("verbose").short("v").multiple(true).help("verbosity level"))
        .args_from_usage("-p, --path=[FILE] 'Target file you want to change'")
        .get_matches();

    if let Some(f) = matches.value_of("path") {
        println!("path : {}", f);
    }
}
