#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}

pub fn add_two(x: i32) -> i32 {
    x + 2
}

pub mod add_two_mod {
    pub fn show_version() {
        println!("version: 0.3.1");
    }
}
