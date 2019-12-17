fn test_if_let() {
    let favorite_color: Option<&str> = None;
    let is_tuesday = false;
    let age: Result<u8, _> = "34".parse();

    if let Some(color) = favorite_color {
        println!("Using your favorite color, {}, as the background", color);
    } else if is_tuesday {
        println!("Tuesday is green day!");
    } else if let Ok(age) = age {
        if age > 30 {
            println!("Using purple as the background color");
        } else {
            println!("Using orange as the background color");
        }
    } else {
        println!("Using blue as the background color");
    }
}

fn test_while_let() {
    let mut stack = Vec::new();

    stack.push(1);
    stack.push(2);
    stack.push(3);

    while let Some(top) = stack.pop() {
        println!("{}", top);
    }
}

fn test_for_loop() {
    let v = vec!['a', 'b', 'c'];

    for (index, value) in v.iter().enumerate() {
        println!("{} is at index {}", value, index);
    }
}

fn test_match_literal() {
    let x = 1;

    match x {
        1 | 2=> println!("one or two"), // multiple pattern
        3 => println!("three"),
        _ => println!("anything"),
    }
}

fn test_match_named_variable() {
    let x = Some(5);
    let y = 10;

    match x {
        Some(50) => println!("Got 50"),
        Some(y) => println!("Matched, y = {:?}", y),
        _ => println!("Default case, x = {:?}", x),
    }

    println!("at the end: x = {:?}, y = {:?}", x, y);
}

// range pattern just apply to number and char
fn test_range_match() {
    let x = 1;

    match x {
        1..=5 => println!("one through five"), // range pattern
        _ => println!("anything"),
    }

    let c = 'y';

    match c {
        'a'..='j' => println!("early ASCII letter"),
        'k'..='z' => println!("late ASCII letter"),
        _ => println!("something else"),
    }
}

#[derive(Debug)]
struct Point {
    x: i32,
    y: i32,
}

fn destruct_struct() {
    let p = Point { x: 0, y: 7 };

    let Point {x: a, y: b } = p;
    println!("{}, {}", a, b);
    println!("{:?}", p);

    let Point {x, y } = p;
    println!("{}, {}", x, y);

    match p {
        Point {x, y: 0} => println!("On the x axis at {}", x),
        Point {x:0, y} => println!("On the y axis at {}", y),
        Point {x, y} => println!("On neither axis: ({}, {})", x, y),
    }
}

enum Color {
    Rgb(i32, i32, i32),
    Hsv(i32, i32, i32),
}

enum Message {
    Quit,
    Move {x: i32, y: i32},
    Write(String),
    ChangeColor(Color),
}

fn destruct_enum() {
    let msg = Message::ChangeColor(Color::Hsv(0, 160, 255));

    match msg {
        Message::Quit => {
            println!("The Quit variant has no data to destruct")
        },
        Message::Move {x, y} => {
            println!("Move in the x direction {} and in the y direction {}", x, y)
        },
        Message::Write(text) => {
            println!("Text message: {}", text)
        },
        Message::ChangeColor(Color::Rgb(r, g, b)) => {
            println!("Change the color to RGB({}, {}, {})", r, g, b)
        }
        Message::ChangeColor(Color::Hsv(h, s, v)) => {
            println!("Change the color to HSV({}, {}, {})", h, s, v)
        }
    }
}

struct Points {
    x: i32,
    y: i32,
    z: i32,
}

fn ignore_remaining() {
    let origin = Points {x: 0, y: 0, z: 0};

    match origin {
        Points {x, .. } => println!("x is {}", x),
    }

    let numbers = (2, 4, 8, 16, 32);

    match numbers {
        (first, .., last) => println!("some number: {}, {}", first, last),
    }
    
    let (f, .., l) = numbers;
    println!("some number: {}, {}", f, l);
}

fn match_guard() {
    let num = Some(4);

    match num {
        Some(x) if x < 5 => println!("less than five: {}", x),
        Some(x) => println!("{}", x),
        None => (),
    }

    let x = 4;
    let y = false;

    match x {
        4 | 5 | 6 if y => println!("yes"),
        _ => println!("no"),
    }
}

enum Msg {
    Hello {id: i32},
}

fn match_bind() {
    let msg = Msg::Hello{ id: 5};

    match msg {
        Msg::Hello{id: id_variable @3..=7} => {
            println!("Found an id in range: {}", id_variable)
        },
        Msg::Hello{id: 10..=12 } => {
            println!("Found an id in another range")
        },
        Msg::Hello{id} if id < 32 || id > 15 => {
            println!("Found an id in another range {}", id)
        },
        Msg::Hello{id} => {
            println!("Found some other range: {}", id)
        },
    }
}

fn main() {
    test_if_let();
    test_while_let();
    test_for_loop();
    test_match_literal();
    test_match_named_variable();
    test_range_match();
    destruct_struct();
    destruct_enum();
    ignore_remaining();
    match_guard();
    match_bind();
}
