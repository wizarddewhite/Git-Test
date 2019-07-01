fn main() {
    let mut number = 3;

    if number < 5 {
        println!("Condition is true.");
    } else {
        println!("Condition is false.");
    }

    let y = if true {
        5
    } else {
        6
    };
    println!("The value of y is {}.", y);

    while number != 0 {
        println!("The value of number is {}.", number);
        number -= 1;
    }

    let a = [10, 20, 30, 40, 50];
    for element in a.iter() {
        println!("The value is: {}", element);
    }

    for n in (1..4).rev() {
        println!("{}", n);
    }
}
