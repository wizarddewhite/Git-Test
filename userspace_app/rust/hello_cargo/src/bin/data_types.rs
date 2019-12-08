fn main(){

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
