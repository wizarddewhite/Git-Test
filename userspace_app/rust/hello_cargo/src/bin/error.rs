
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

use std::error::Error;
pub struct Guess {
    value: Option<i32>,
}

impl Guess {
    fn new(value: i32) -> Guess {
        if value > 1 && value < 100 {
            Guess {value: Some(3)}
        } else {
            Guess {value: None}
        }
    }

    // pub fn value(&self) -> i32 {
        // self.value
    // }
}

fn test_new_guest() {
    let g = Guess::new(500);

    match g.value {
        Some(v) => println!("vvv {}", v),
        _ => println!("not a valid guess"),
    }
}

// Define your own error
pub enum OwnError {
    Start,
    Stop,
    Wait(i32),
    Pause(i32),
}

fn return_err() -> Result<i32, ErrorKind> {
    Err(ErrorKind::NotFound)
}

fn return_wait() -> Result<i32, i32> {
    Err(5)
}

// map an error from one type to a user defined one
fn test_map_err() -> Result<i32, OwnError> {
    return_err().map_err(|_| OwnError::Start)?;

    // when e has the same type wrapped by enum, we can simplify the code
    // so the following two line are the same
    // return_wait().map_err(|e| OwnError::Wait(e));
    return_wait().map_err(OwnError::Wait)
}

fn main() {
    test_new_guest();
    test_recoverable();
    test_unrecoverable();
}