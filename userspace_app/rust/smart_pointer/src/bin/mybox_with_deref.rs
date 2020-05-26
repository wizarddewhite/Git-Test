#[derive(Debug)]
struct MyBox<T, U>(T, U);

impl<T, U> MyBox<T, U> {
    fn new(x: T, y: U) -> MyBox<T, U> {
        MyBox(x, y)
    }
}

// implement Deref trait
use std::ops::Deref;

impl<T, U> Deref for MyBox<T, U> {
    type Target = U;

    fn deref(&self) -> &U {
        &self.1
    }
}

fn main() {
    let x = 5;
    let y = MyBox::new(x, 4);
    println!("y is {:?}", y);

    assert_eq!(5, x);
    // *y equals to *(y.deref())
    assert_eq!(4, *y);
    println!("{}", *y); // now it will print 4
}
