use std::sync::Mutex;

fn test_mutex() {
    let m = Mutex::new(5);

    {
        let mut num = m.lock().unwrap();
        *num = 6;
    }

    println!("m = {:?}", m);
}

use std::thread;
use std::sync::Arc;
use std::time::Duration;

fn thread_mutex() {
    let counter = Arc::new(Mutex::new(0));
    let mut handles = vec![];
    println!("Start: {}", *counter.lock().unwrap());

    for _ in 0..10 {
        let counter = Arc::clone(&counter);
        let handle = thread::spawn(move || {
            let mut num = counter.lock().unwrap();

            *num += 1;
            println!("   changing to: {}", num);
            thread::sleep(Duration::from_millis(10));
        });
        handles.push(handle);
    }

    for handle in handles {
        handle.join().unwrap();
    }

    println!("Result: {}", *counter.lock().unwrap());
}

fn main() {
    println!("### simple usage");
    test_mutex();
    println!("### use mutex to sync threads");
    thread_mutex();
}
