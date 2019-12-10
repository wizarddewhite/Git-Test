fn simple_type() {
    // Scalar types: 
    // * integers
    // * floating-point numbers
    // * Booleans
    // * characters 

    // integer
    let _a: u32 = 10_000;
    let _b: u64 = 100_000;
    println!("The value of a is {}", _a);
    // float
    let _c: f32 = 2.0;
    let _d: f64 = 3.0;
    // boolean
    let _e: bool = true;
    // character, 4 bytes
    let _f: char = 'z';


    // Compound types:
    // * tuples 
    // * arrays

    // tuple, fixed length
    let tup: (i32, f64, u8) = (500, 6.4, 1);
    // destruct tup into separate variable
    let (_, y, _) = tup;
    println!("The value of y is {}", y);
    // access tup by using a period (.)
    println!("The value of y is {}", tup.1);


    // array, must has the same type and fixed length
    // basic definition
    let a = [1, 2, 3, 4, 5];
    println!("The value of a[0] is {}", a[0]);
    // specify type and length
    let b: [i32; 5] = [1, 2, 3, 4, 5];
    println!("The value of b[0] is {}", b[0]);
    // an array with 5 same value: "3"
    let arr: [i32; 5] = [3; 5];
    println!("The value of arr[0] is {}", arr[0]);
}

fn test_vector() {
    println!("### Test on vector");
    // define a vector
    let _v1: Vec<i32> = Vec::new();
    let _v2 = vec![1, 2, 3];

    // push
    let mut v3 = Vec::new();
    v3.push(5);
    v3.push(6);
    v3.push(7);
    v3.push(8);

    // reading
    let v4 = vec![1, 2, 3, 4, 5];
    let third = &v4[2];
    println!("The third element is {}", third);

    match v4.get(2) {
        Some(num) => println!("The third element is {}", num),
        None => println!("There is no third element"),
    }

    // iterate
    println!("Original vec:");
    let mut v5 = vec![100, 32, 57];
    for i in &v5 {
        println!("{}", i);
    }

    for i in &mut v5 {
        *i += 50;
    }
    println!("After +50:");
    for i in &v5 {
        println!("{}", i);
    }

}

fn test_string() {
    // define a string
    let _s1 = "initial contents".to_string();
    let _s2 = String::from("initial contents");

    // update a string
    let mut s1 = String::from("foo");
    s1.push_str(" bar");
    println!("{}", s1);

    let s2 = "abc";
    s1.push_str(s2); // push_str() doesn't take ownership
    println!("{}", s2);

    // concatenation
    let s3 = String::from("Hello, ");
    let s4 = String::from("world!");
    let s5 = s3 + &s4; // note s3 has been moved here and can no longer be used
    println!("{}", s5);

    let s6 = format!("{}{}", "Hello, ", "world!");
    println!("{}", s6);

    // iterate as char
    for c in "haha".chars() {
        println!("{}", c)
    }
    // iterate as byte
    for c in "haha".bytes() {
        println!("{}", c)
    }
}

use std::collections::HashMap;

fn test_hashmap() {

    // define
    let mut scores = HashMap::new();

    scores.insert(String::from("Blue"), 10);
    scores.insert(String::from("Yellow"), 50);

    let color = vec![String::from("Red"), String::from("Green")];
    let val = vec![20, 40];

    let mut scores2: HashMap<_, _> = color.iter().zip(val.iter()).collect();

    // get a value
    let s = scores2.get(&String::from("Red"));
    println!("Red's score: {:#?}", s);

    // iterate
    for (key, value) in &scores2 {
        println!("{}: {}", key, value);
    }

    // overwrite
    scores.insert(String::from("Blue"), 50);
    scores2.insert(&String::from("Blue"), &50); // why need to use &?

    // insert when key is not there
    scores.entry(String::from("Red")).or_insert(20); // insert [Red, 20]
    scores.entry(String::from("Blue")).or_insert(20); // no change

    // update based on old value
    let text = "hello world wonderful world";

    let mut map = HashMap::new();

    for word in text.split_whitespace() {
        let count = map.entry(word).or_insert(0);
        *count += 1;
    }
    println!("{:?}", map);
}

fn main(){
    simple_type();
    test_vector();
    test_string();
    test_hashmap();
}
