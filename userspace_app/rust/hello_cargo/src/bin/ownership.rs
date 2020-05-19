#![allow(unused_variables)]

// Refer to: https://doc.rust-lang.org/book/ch04-01-what-is-ownership.html

// ownership rules
// * each value in Rust has a variable that's called its owner
// * there can only be one owner at a time
// * when the owner goes out of scope, the value will be dropped

fn test_scope() {
    // scope
    {   // s is not valid here
        let s = "hello";  // s is valid from this point
        // do stuff with s
    }   // this scope is now over, and s is no longer valid

    {
        // s is allocated from heap at runtime
        let mut s = String::from("hello");
        s.push_str(", world!");
        println!("{}", s);
    }

}

fn test_move() {
    // simple data
    {
        // both x and y are on stack
        let x = 5;
        let y = x;  // not need to move for variables on stack
        println!("y = x doesn't move x since x is on stack\n\tx = {}, y = {}", x, y);
    }

    // variable move
    {
        //    Stack                          Heap
        // s1
        // +-----------+
        // |ptr        |----------+      
        // +-----------+          |      +------------------+
        // |len        |          +----->|hello             |
        // +-----------+          |      +------------------+
        // |cap        |          |
        // +-----------+          |
        //                        |
        // s2                     |
        // +-----------+          |
        // |ptr        |----------+      
        // +-----------+
        // |len        |
        // +-----------+
        // |cap        |
        // +-----------+
        //
        // For data type like String, the value s1 and s2 are on stack,
        // while the content is on heap.
        let s1 = String::from("hello");
        let s2 = s1;  // s1 is no longer valid, s1 is moved to s2

        // println!("{}, world!", s1);  // Error: value used after move
        println!("s2 = s1 move s1 ownership since s1 is a String on heap.");
        println!("\ts2 = {}, world!", s2);

        // if not invalidate s1, we need to free s1 and s2,
        // which leads to double free.
    }

    // data clone
    {

        //    Stack                          Heap
        // s1
        // +-----------+                 +------------------+
        // |ptr        |---------------->|hello             |
        // +-----------+                 +------------------+
        // |len        |          
        // +-----------+          
        // |cap        |           
        // +-----------+           
        //                         
        // s2                      
        // +-----------+                 +------------------+ 
        // |ptr        |---------------->|hello             |       
        // +-----------+                 +------------------+
        // |len        |
        // +-----------+
        // |cap        |
        // +-----------+
        //
        let s1 = String::from("hello");
        let s2 = s1.clone();
        println!("s1 is valid after s2 = s1.clone()");
        println!("\ts1 {}, world!", s1);
        println!("\ts2 {}, world!", s2);
    }
}

fn takes_ownership(some_string: String) { // some_string comes into scope
    println!("\t\"{}\" ownership is taken since this is a String variable", some_string);
    println!("\t and the .drop() is called to free memory");
} // Here, some_string goes out of scope and `drop` is called. The backing
  // memory is freed.

fn makes_copy(some_integer: i32) { // some_integer comes into scope
    println!("\t\"{}\": i32 variable is taken", some_integer);
} // Here, some_integer goes out of scope. Nothing special happens.

fn test_func_param() {
    let s = String::from("hello");  // s comes into scope

    println!("we can't access s after takes_ownership(s: String)");
    takes_ownership(s);             // s's value moves into the function...
                                    // ... and so is no longer valid here

    let x = 5;                      // x comes into scope

    println!("we can access x after makes_copy(x: i32) since i32 is Copy");
    makes_copy(x);                  // x would move into the function,
                                    // but i32 is Copy, so it’s okay to still
                                    // use x afterward

} // Here, x goes out of scope, then s. But because s's value was moved, nothing
  // special happens.

fn gives_ownership() -> String {             // gives_ownership will move its
                                             // return value into the function
                                             // that calls it

    let some_string = String::from("hello"); // some_string comes into scope

    some_string                              // some_string is returned and
                                             // moves out to the calling
                                             // function
}

// takes_and_gives_back will take a String and return one
fn takes_and_gives_back(a_string: String) -> String { // a_string comes into
                                                      // scope

    a_string  // a_string is returned and moves out to the calling function
}

fn test_func_return_value() {
    let s1 = gives_ownership();         // gives_ownership moves its return
                                        // value into s1

    let s2 = String::from("hello");     // s2 comes into scope

    println!("s2's ownership is passed to takes_and_gives_back()");
    let s3 = takes_and_gives_back(s2);  // s2 is moved into
                                        // takes_and_gives_back, which also
                                        // moves its return value into s3
    println!("\tso we can access s3 \"{}\", but not s2", s3);
} // Here, s3 goes out of scope and is dropped. s2 goes out of scope but was
  // moved, so nothing happens. s1 goes out of scope and is dropped.

fn calculate_length(s: &String) -> usize {
    // get s without taking ownership, which is called borrow
    println!("\tcalculate_length(s: &String) doesn't take ownership");
    s.len()
} // Here, s goes out of scope. But because it does not have ownership of what
  // it refers to, nothing happens.

fn test_reference() {
    let s1 = String::from("hello");

    // pass reference to function, called borrowing
    // &s1 create a reference without own it
    println!("We can still access s1, since calculate_length(s: &String) takes reference.");
    let len = calculate_length(&s1);

    // still could use s1 here
    println!("\tThe length of '{}' is {}.", s1, len);
}

fn change(some_string: &mut String) {
    println!("\tpush \"world\" to s");
    some_string.push_str(", world");
}

fn test_mutable_reference() {
    let mut s = String::from("hello");

    println!("We can adjust variable with mutable reference. {}", s);
    change(&mut s);
    println!("\tis changed to {}", s);
}

fn forbid_double_mut_ref() {
    let mut s = String::from("hello");

    println!("Only one mutable reference is allowed.");
    let r1 = &mut s; // no problem
    // you can have only one mutable reference to a particular piece of data
    // in a particular scope
    //
    // let r2 = &mut s; // no problem
    // println!("{} {}", r1, r2);
}

/*
fn dangle() -> &String { // dangle returns a reference to a String

    let s = String::from("hello"); // s is a new String

     &s // we return a reference to the String, s
} // Here, s goes out of scope, and is dropped. Its memory goes away.
  // Danger!
*/
fn no_dangle() -> String {
    let s = String::from("hello");

    s
}

fn ref_in_ref_out(some_string: &String) -> &String {
    some_string
}

fn test_ref_inout() {
    let s = String::from("hello");
    let rs = ref_in_ref_out(&s);
    println!("{}", rs);
}

fn first_word(s: &String) -> &str {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[..i];
        }
    }

    return &s[..];
}

fn first_word2(s: &str) -> &str {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[..i];
        }
    }

    return &s[..];
}

fn two2three_elem(a: &[i32]) -> &[i32] {
    if a.len() >= 3 {
        return &a[1..3];
    }
    return &a[..];
}

fn test_strig_slice() {
    let mut s = String::from("hello world");

    let hello = &s[..5]; // A string slice is a reference to part of a String.
    let world = &s[6..];

    println!("Play with slice:");
    println!("\ts[..5]: {}", hello);
    println!("\ts[6..]: {}", world);

    let word = first_word(&s);
    println!("\tword retrieved from s: {} ", word);
    let word2 = first_word2(&s[..]);
    println!("\tword2 retrieved from s: {} ", word2);
    println!("\tAfter this we could still access s: {} ", s);

    s.clear();
    println!("\tCan't access word retrieved from s, since s.clear() is mutable borrow and word is immutable borrow");

    let a = [1, 2, 3, 4];
    println!("\t [1..3] elements of {:?} is {:?}", a, two2three_elem(&a));
}

fn main() {

    test_scope();
    test_move();
    test_func_param();
    test_func_return_value();
    test_reference();
    test_mutable_reference();
    forbid_double_mut_ref();

    no_dangle();
    test_ref_inout();

    test_strig_slice();
}
