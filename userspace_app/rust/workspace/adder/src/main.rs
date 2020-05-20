use add_one;
use add_two;

fn main() {
    let num = 10;
    println!("{}", add_one::add_one(num));
    println!("{}", add_two::add_two(num));

    add_two::add_two_mod::show_version();
}
