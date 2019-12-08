#![allow(unused_variables)]
const MAX_POINTS: u32 = 100_000;
fn main() {
    // immutable
    let x = 5;
    println!("The value of x is: {}", x);
    // error: can't assign twice to immutable variables
    //x = 6;
    //println!("The value of x is: {}", x);

    // constant
    println!("The value of MAX_POINTS is: {}", MAX_POINTS);

    // mutable
    let mut y = 5;
    println!("The value of y is: {}", y);
    y = 6;
    println!("The value of y is: {}", y);

    // shadow
    let z = 5;
    let z = z + 1;
    let z = z * 2;
    println!("The value of z is: {}", z);

    let spaces = "    ";
    let spaces = spaces.len();
    println!("The space length is: {}", spaces);
}
