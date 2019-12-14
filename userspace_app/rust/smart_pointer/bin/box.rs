// simple test
fn simple_usage() {
    let b = Box::new(5);
    println!("b = {}", b);

    let bc = Box::into_raw(b);
    println!("bc = {:?}", bc);
}

// define recursive data type
enum List {
    Cons(i32, Box<List>),
    Nil,
}

use crate::List::{Cons, Nil};

fn recursive_type() {
    let list = Cons(1,
        Box::new(Cons(2,
            Box::new(Cons(3,
                Box::new(Nil))))));
    //let mut con = list;
    //loop {
    //    con = match con {
    //        Cons(val, next) => {
    //            println!("{}", val);
    //            next.downcast::<List>()
    //        },
    //        Nil => break,
    //    };
    //}
}

fn main() {
    simple_usage();
    recursive_type();
}
