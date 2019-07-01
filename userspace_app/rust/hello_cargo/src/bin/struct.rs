#[derive(Debug)]
struct User {
    username: String,
    email: String,
    sign_in_count: u64,
    active: bool,
}

fn build_user(email: String, username: String) -> User {
    User {
        //email: email,
        email,
        username: username,
        active: true,
        sign_in_count: 1,
    }
}

// tuple struct
struct Color(i32, i32, i32);
struct Point(i32, i32, i32);

fn main() {
    let _user1 = build_user(String::from("someone@emample.com"), 
                           String::from("someone"));
    let user2 = User {
        email: String::from("another@example.com"),
        username: String::from("another"),
        .._user1
    };
    println!("user2 is {:#?}", user2);

    let mut black = Color(0,0,0);
    println!("Black.0 is {}", black.0);

    let rect = Rectangle {width: 30, height: 50};
    println!("Area of rectangle {}", rect.area());
    let rect2 = Rectangle {width: 10, height: 40};
    println!("Can rect hold rect2? {}", rect.can_hold(&rect2));
    let sq = Rectangle::square(10);
    println!("Area of square {}", sq.area());
}

struct Rectangle {
    width: u32,
    height: u32,
}

impl Rectangle {
    fn area(&self) -> u32 {
        self.width * self.height
    }

    fn can_hold(&self, other: &Rectangle) -> bool {
        self.width > other.width && self.height > other.height
    }
}

impl Rectangle {
    fn square(size: u32) -> Rectangle {
        Rectangle {width: size, height: size}
    }
}
