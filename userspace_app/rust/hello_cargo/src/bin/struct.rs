#[derive(Debug)]
// define custom type with struct
struct User {
    username: String,
    email: String,
    sign_in_count: u64,
    active: bool,
}

fn build_user(email: String, username: String) -> User {
    // instance a variable with type struct User
    User {
        //email: email,
        email, // could be simplified since they have the same name
        username: username,
        active: true,
        sign_in_count: 1,
    }
}

// some example on struct type
fn test_struct() {
    // function with return value of type struct
    let _user1 = build_user(String::from("someone@example.com"), 
                           String::from("someone"));
    let email = String::from("another@example.com");
    let username = String::from("another");
    let user2 = User {
        email,
        username,
        // active: user1.active,
        // sign_in_count: user1.sign_in_count,
        .._user1  // up two lines be simplified for Struct Update syntax
    };
    println!("user2 is {:#?}", user2);
}

// tuple struct, a more verbose way to use tuple
struct Color(i32, i32, i32);
struct Point(i32, i32, i32);

fn test_tuple_struct() {
    let black = Color(0, 0, 0);
    println!("Black.0 is {}", black.0);
    let point = Point(1, 2, 3);
    println!("Point.0 is {}", point.0);
}

#[derive(Debug)]
struct Rectangle {
    width: u32,
    height: u32,
}

impl Rectangle {
    // self must be the first parameter
    fn area(&self) -> u32 {
        self.width * self.height
    }

    fn can_hold(&self, other: &Rectangle) -> bool {
        self.width > other.width && self.height > other.height
    }
}

// define associated functions, no self is required
impl Rectangle {
    fn square(size: u32) -> Rectangle {
        Rectangle {width: size, height: size}
    }
}

fn example_for_struct() {
    let rect = Rectangle {width: 30, height: 50};
    // print rect with Debug format
    println!("rect is {:?}", rect);
    // print rect with pretty Debug format
    println!("rect is {:#?}", rect);
    println!("Area of rectangle {}", rect.area());
    let rect2 = Rectangle {width: 10, height: 40};
    println!("Can rect hold rect2? {}", rect.can_hold(&rect2));
    let sq = Rectangle::square(10);
    println!("Area of square {}", sq.area());
}

fn main() {

    test_struct();
    test_tuple_struct();
    example_for_struct();
}
