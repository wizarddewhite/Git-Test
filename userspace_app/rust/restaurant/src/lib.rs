#[cfg(test)]
mod front_of_house {
    mod hosting {
        fn add_to_waitlist() { }
        fn seat_at_table() { }
    }
    mod serving {
        fn take_over() {}
        fn serve_order() {}
        fn take_payment() {}
    }
}
