fn main(){
    // integer
    let _a: u32 = 10_000;
    let _b: u64 = 100_000;
    // float
    let _c: f32 = 2.0;
    let _d: f64 = 3.0;
    // boolean
    let _e: bool = true;
    // characer, 4 bytes
    let _f: char = 'z';


    // tuple
    let tup: (i32, f64, u8) = (500, 6.4, 1);
    let (_, y, _) = tup;
    println!("The value of y is {}", y);
    println!("The value of y is {}", tup.1);
    // array
    let arr: [i32; 5] = [3; 5];
    println!("The value of arr[0] is {}", arr[0]);
}
