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
    println!("The value of 3 - 4 is: {}.", del(3, 4));
}

fn another_function() {
    println!("Another function.");
}

fn print_integer(x: i32) {
    println!("The value of x is: {}.", x);
}

// Statement and Expression
// Statement:
//
//   E.g. let y = 6;
//
//   Statement do not return values.
//
// Expression:
//
//   E.g. x + 1
//   Note: x + 1;  is a statement instead of expression.
//
//   Expression return values.

// function with return value.
// The return value of the function is synonymous with the value of the final
// EXPRESSION in the block of the body of a function.
fn add(x: i32, y: i32) -> i32 {
    x + y  // Should be an expression, so no ";" here
}

fn del(x: i32, y: i32) -> i32 {
    return x - y;
}
