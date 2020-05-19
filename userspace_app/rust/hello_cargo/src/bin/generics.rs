// Refer to: https://doc.rust-lang.org/book/ch10-01-syntax.html
fn largest_i32(list: &[i32]) -> i32 {
    let mut largest = list[0];

    for &item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}

fn largest_char(list: &[char]) -> char {
    let mut largest = list[0];

    for &item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}

// function with generic type
// <T: PartialOrd + Copy> is trait bound syntax
// Refer to https://doc.rust-lang.org/book/ch10-02-traits.html
fn largest<T: PartialOrd + Copy>(list: &[T]) -> T {
    let mut largest = list[0];

    for &item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}

fn smallest<T>(list: &[T]) -> T 
    where T: PartialOrd + Copy
{
    let mut smallest = list[0];

    for &item in list.iter() {
        if item < smallest {
            smallest = item;
        }
    }

    smallest
}

fn test_generic_function() {
    let number_list = vec![34, 50, 25, 100, 65];
    println!("The largest number is {}", largest_i32(&number_list));
    println!("Origin list: {:?}", number_list);

    let number_list = vec![102, 34, 6000, 89, 54, 2, 43, 8];
    println!("The largest number is {}", largest_i32(&number_list));
    println!("Origin list: {:?}", number_list);

    let char_list = vec!['y', 'm', 'a', 'q'];
    println!("The largest char is {}", largest_char(&char_list));
    println!("Origin list: {:?}", char_list);

    println!("The largest char is {}", largest(&char_list));
    println!("The smallest char is {}", smallest(&char_list));
}

// struct with generic
struct Point<T> {
    x: T,
    y: T,
}

fn test_generic_struct() {
    let _integer = Point {x: 5, y: 10};
    let _float = Point {x: 1.0, y: 4.0};
}

// method with generic
impl<T> Point<T> {
    fn x(&self) -> &T {
        &self.x
    }
}

// implement specially for type f32
impl Point<f32> {
    fn distance_from_origin(&self) -> f32 {
        (self.x.powi(2) + self.y.powi(2)).sqrt()
    }
}

#[derive(Debug)]
struct Rectant<T, U> {
    x: T,
    y: U,
}

// some generic parameters are declared with impl
// and some are declared with the method definition
impl<T, U> Rectant<T, U> {
    fn mixup<V, W>(self, other: Rectant<V, W>) -> Rectant<T, W> {
        Rectant {
            x: self.x,
            y: other.y,
        }
    }
}


fn test_generic_method() {
    let p = Point {x: 5, y:10};

    println!("Generic Type Struct Pointer<T>: p.x = {}", p.x());

    let p1 = Rectant { x: 5, y: 10.4 };
    let p2 = Rectant { x: "Hello", y: 'c' };

    let p3 = p1.mixup(p2);

    println!("Generic Type Rectant: p3.x = {}, p3.y = {}", p3.x, p3.y);
    println!("Since mixup() move ownership, p1 and p2 can't be access now");
}

#[derive(Debug)]
enum GenericEnum<T> {
    First(T),
    Second(T),
}

fn test_generic_enum() {
    let f = GenericEnum::First(5);

    match f {
        GenericEnum::First(val) => {
            println!("This is a First with value: {:?}", val)
        },
        _ => (),
    }
}

fn main() {
    test_generic_function();
    test_generic_struct();
    test_generic_method();
    test_generic_enum();
}
