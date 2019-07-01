#[derive(Debug)]
enum IpAddrKind {
    V4,
    V6,
}

struct IpAddr {
    kind: IpAddrKind,
    address: String,
}

fn route(ip_kind: IpAddrKind) {}

enum Message {
    Quit,
    Move {x: i32, y: i32},
    Write(String),
    ChangeColor(i32, i32, i32),
}

impl Message {
    fn call(&self) {
        //println!("Message is: {:#?}", self);
        println!("Message is: ");
    }
}

enum UsState {
    Alabama,
    Alaska,
}

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
            //println!("State quarter from {:?}!", state);
            25
        },
    }
}

fn main() {
    let four = IpAddrKind::V4;
    route(four);

    let m = Message::Write(String::from("hello"));
    m.call();

    println!("cents: {}", value_in_cents(Coin::Dime));
    println!("cents: {}", value_in_cents(Coin::Quarter(UsState::Alabama)));
}
