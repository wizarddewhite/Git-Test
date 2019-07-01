fn main() {
    println!("Hello, world!");
    another_function();
    print_integer(5);

    // statement
    let y = {
        let x = 3;
        x + 1      // no ; here
    };
    println!("The value of y is: {}.", y);

    println!("The value of 3 + 4 is: {}.", add(3, 4));
}

fn another_function() {
    println!("Another function.");
}

fn print_integer(x: i32) {
    println!("The value of x is: {}.", x);
}

fn add(x: i32, y: i32) -> i32 {
    x + y  // no ; here
}
