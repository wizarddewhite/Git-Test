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
fn largest<T: PartialOrd + Copy>(list: &[T]) -> T {
    let mut largest = list[0];

    for &item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
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

fn test_generic_method() {
    let p = Point {x: 5, y:10};

    println!("p.x = {}", p.x());
}
fn main() {
    test_generic_function();
    test_generic_struct();
    test_generic_method();
}
