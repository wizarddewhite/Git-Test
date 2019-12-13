fn main() {
    let v1 = vec![1, 2, 3];

    let v1_iter = v1.iter();
    
    for val in v1_iter {
        println!("Got: {}", val);
    }

    // use .next method
    let mut iter = v1.iter();
    loop {
        match iter.next() {
            Some(num) => println!("Has: {}", num),
            None => break,
        }
    }

    // sum()
    let iter2 = v1.iter();

    let total: i32 = iter2.sum();
    println!("Total sum: {}", total);

    // map()
    let v2: Vec<_> = v1.iter().map(|x| x + 1).collect();
    println!("Vec: {:?}", v2);

    // filter()
    filters_by_size();

    // own iterator
    test_own_iterator();
}

#[derive(PartialEq, Debug)]
struct Shoe {
    size: u32,
    style: String,
}

fn shoes_in_my_size(shoes: Vec<Shoe>, shoe_size: u32) -> Vec<Shoe> {
    shoes.into_iter()
        .filter(|s| s.size == shoe_size)
        .collect()
}

fn filters_by_size() {
    let shoes = vec![
        Shoe { size: 10, style: String::from("sneaker") },
        Shoe { size: 13, style: String::from("sandal") },
        Shoe { size: 10, style: String::from("boot") },
    ];

    let in_my_size = shoes_in_my_size(shoes, 10);
    println!("My size shoes: {:?}", in_my_size);
}

// own iterator
#[derive(Debug)]
struct Counter {
    count: u32
}

impl Counter {
    fn new() -> Counter {
        Counter { count: 0}
    }
}

impl Iterator for Counter {
    type Item = u32;

    fn next(&mut self) -> Option<Self::Item> {
        self.count += 1;

        if self.count < 6 {
            Some(self.count)
        } else {
            None
        }
    }
}

fn test_own_iterator() {
    let mut counter = Counter::new();

    loop {
        match counter.next() {
            Some(num) => println!("Counter: {}", num),
            None => break,
        }
    }

    let counter2 = Counter::new();

    let val = counter2.skip(1);
    println!("Counter skip 1 {:?}", val);
}
