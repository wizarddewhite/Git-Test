enum List {
    Cons(i32, Rc<List>),
    Nil,
}

use crate::List::{Cons, Nil};
use std::rc::Rc;

fn test_rc() {
    let a = Rc::new(Cons(5, Rc::new(Cons(10, Rc::new(Nil)))));
    println!("count after creating a = {}", Rc::strong_count(&a));
    let b = Cons(3, Rc::clone(&a));
    println!("count after creating b = {}", Rc::strong_count(&a));
    {
        let c = Cons(4, Rc::clone(&a));
        println!("count after creating c = {}", Rc::strong_count(&a));
    }
    println!("count after c goes out of scope = {}", Rc::strong_count(&a));
}

#[derive(Debug)]
enum List2 {
    Cons2(Rc<RefCell<i32>>, Rc<List2>),
    Nil2,
}

impl List2 {
    fn val(&self) -> Option<&Rc<RefCell<i32>>> {
        match self {
            Cons2(item, _) => Some(item),
            Nil2 => None,
        }
    }
}

use crate::List2::{Cons2, Nil2};
use std::cell::RefCell;

fn test_rc_refcell() {
    let value = Rc::new(RefCell::new(5));
    println!("value.strong_count after creating = {}",
             Rc::strong_count(&value));

    let a = Rc::new(Cons2(Rc::clone(&value), Rc::new(Nil2)));
    println!("value.strong_count after clone for a = {}",
             Rc::strong_count(&value));
    println!("a origin = {:?}", a);

    let b = Cons2(Rc::new(RefCell::new(6)), Rc::clone(&a));
    let c = Cons2(Rc::new(RefCell::new(10)), Rc::clone(&a));

    *value.borrow_mut() += 10;

    println!("a after = {:?}", a);
    println!("b after = {:?}", b);
    println!("c after = {:?}", c);

    // modify a's value
    if let Some(val) = a.val() {
        *val.borrow_mut() += 1;
    }
    println!("a after = {:?}", a);
}

fn main() {
    println!("### Test RC");
    test_rc();
    println!("### Test RC & RefCell");
    test_rc_refcell();
}
