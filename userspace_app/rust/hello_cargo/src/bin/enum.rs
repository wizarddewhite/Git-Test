#[derive(Debug)]

// define enum type IpAddrKind
enum IpAddrKind {
    V4,
    V6,
}

// define a function with input type IpAddrKind
fn route(_ip_kind: IpAddrKind) {}

// use enum as a filed in struct
#[derive(Debug)]
struct IpAddr {
    kind: IpAddrKind,
    address: String,
}

fn test_ipaddr() {
    // define enum variable
    let four = IpAddrKind::V4;
    let six = IpAddrKind::V6;
    // invoke the route() with enum
    route(four);
    route(six);

    // instance an IpAddr
    let home = IpAddr {
        kind: IpAddrKind::V4,
        address: String::from("127.0.0.1"),
    };

    println!("home address is {:#?}", home);
}

// putting data directly into each enum variant
#[derive(Debug)]
enum IpAddr2 {
    V4(String),
    V6(String),
}

fn test_ipaddr2() {
    let home = IpAddr2::V4(String::from("127.0.0.1"));
    let _loopback = IpAddr2::V6(String::from("::1"));
    println!("home address is {:#?}", home);

    let addr = match home {
        IpAddr2::V4(addr) => addr,
        IpAddr2::V6(addr) => addr,
    };
    println!("home address is {}", addr);
}

#[derive(Debug)]
enum IpAddr3 {
    V4(u8, u8, u8, u8),
    V6(String),
}

fn test_ipaddr3() {
    let home = IpAddr3::V4(127, 0, 0, 1);
    let _loopback = IpAddr3::V6(String::from("::1"));
    println!("home address is {:#?}", home);
}

#[derive(Debug)]
enum Message {
    Quit,
    Move {x: i32, y: i32},
    Write(String),
    ChangeColor(i32, i32, i32),
}

impl Message {
    fn call(&self) {
        println!("Message is: {:#?}", self);
    }
}

fn test_message() {
    let m = Message::Write(String::from("hello"));
    m.call();
}

#[derive(Debug)]
enum UsState {
    Alabama,
    Alaska,
}

#[derive(Debug)]
enum Coin {
    Penny,
    Nickel,
    Dime,
    Quarter(UsState),
}

fn value_in_cents(coin: Coin) -> u8 {
    match coin {
        Coin::Penny => 1,
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter(state) => {
            println!("\tState quarter from {:?}!", state);
            25
        },
    }
}

fn test_match_coin() {
    println!("cents: {}", value_in_cents(Coin::Dime));
    println!("cents: {}", value_in_cents(Coin::Quarter(UsState::Alabama)));
}

fn test_placeholder() {
    let some_u8_value = 5u8;
    // match require to list all possible values
    // while we can ignore rest by _ placeholder
    match some_u8_value {
        1 => println!("one"),
        3 => println!("three"),
        5 => println!("five"),
        7 => println!("seven"),
        _ => (),
    }

    // in case we just want to match one case,
    // we can use if let
    if let 4u8 = some_u8_value {
        println!("if let returns 5");
    } else {
        println!("if let not match");
    }
}

fn main() {

    test_ipaddr();
    test_ipaddr2();
    test_ipaddr3();

    test_message();

    test_match_coin();

    test_placeholder();
}
