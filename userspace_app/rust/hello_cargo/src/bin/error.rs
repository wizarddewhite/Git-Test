
// unrecoverable error
fn test_unrecoverable() {
    // panic!
    // panic!("crash and burn");
    
    // out of boundary
    let v = vec![1, 2, 3];
    v[99];
}

use std::fs::File;
use std::io::ErrorKind;
// recoverable error with Result
fn test_recoverable() {
    let f = File::open("hello.txt");

    let f = match f {
        Ok(file) => file,
        Err(error) => match error.kind() {
            ErrorKind::NotFound => match File::create("hello.txt") {
                Ok(fc) => fc,
                Err(e) => panic!("Problem creating the file: {:?}", e),
            },
            other_error => panic!("Problem opening the file: {:?}", other_error),
        },
    };

    // panic with provided message
    let f2 = File::open("hello.txt").expect("Failed to open hello.txt");

}

use std::io;
use std::io::Read;
// propagating errors
fn read_username_from_file() -> Result<String, io::Error> {
    let f = File::open("hello.txt");

    let mut f = match f {
        Ok(file) => file,
        Err(e) => return Err(e),
    };

    let mut s = String::new();

    match f.read_to_string(&mut s) {
        Ok(_) => Ok(s),
        Err(e) => Err(e),
    }
}

// shorten with ?
fn read_username_from_file2() -> Result<String, io::Error> {
    let mut s = String::new();

    File::open("hello.txt")?.read_to_string(&mut s)?;

    Ok(s)
}

fn main() {
    test_recoverable();
    test_unrecoverable();
}
