use std::rc::{Rc, Weak};
use std::cell::RefCell;

#[derive(Debug)]
struct Node {
    value: i32,
    parent: RefCell<Weak<Node>>,
    children: RefCell<Vec<Rc<Node>>>,
}

fn main() {
    let leaf = Rc::new(Node {
        value: 3,
        parent: RefCell::new(Weak::new()),
        children: RefCell::new(vec![]),
    });
    println!("leaf parent = {:?}", leaf.parent.borrow().upgrade());
    println!(
        "leaf on creation strong = {}, weak = {}",
        Rc::strong_count(&leaf),
        Rc::weak_count(&leaf)
        );

    {
        let branch = Rc::new(Node{
            value: 5,
            parent: RefCell::new(Weak::new()),
            children: RefCell::new(vec![Rc::clone(&leaf)]),
        });
        println!(
            "branch on creation strong = {}, weak = {}",
            Rc::strong_count(&branch),
            Rc::weak_count(&branch)
            );
    
        *leaf.parent.borrow_mut() = Rc::downgrade(&branch);
        println!("leaf parent = {:?}", leaf.parent.borrow().upgrade());
        println!(
            "leaf after assign strong = {}, weak = {}",
            Rc::strong_count(&leaf),
            Rc::weak_count(&leaf)
            );
    
        println!(
            "branch after assign strong = {}, weak = {}",
            Rc::strong_count(&branch),
            Rc::weak_count(&branch)
            );
    }

    println!("leaf parent = {:?}", leaf.parent.borrow().upgrade());
    println!(
        "leaf out scope strong = {}, weak = {}",
        Rc::strong_count(&leaf),
        Rc::weak_count(&leaf)
        );
}
