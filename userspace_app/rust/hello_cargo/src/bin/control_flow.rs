fn main() {
    let mut number = 3;

    // if condition
    if number < 5 {
        println!("Condition is true.");
    } else {
        println!("Condition is false.");
    }

    // assign value conditionally
    let y = if true {
        5
    } else {
        6
    };
    // y should be 5
    println!("The value of y is {}.", y);

    // loop loop
    loop {
        println!("The value of number is {}.", number);
        number += 1;
        if number >= 3 {
            break;
        }
    }

    // loop with return value in break
    let result = loop {
        number += 1;
        break number * 2; // number * 2 is the result
    };
    println!("The result from loop is: {}", result);

    // conditional loop with while
    while number != 0 {
        println!("The value of number is {}.", number);
        number -= 1;
    }

    // for loop on an array
    let a = [10, 20, 30, 40, 50];
    for element in a.iter() {
        println!("The value is: {}", element);
    }

    // for loop on a range [3, 2, 1]
    for n in (1..4).rev() {
        println!("{}", n);
    }
}
