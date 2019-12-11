// generic lifetime in function
// * parameter x and y live at least as long as lifetime 'a
// * return value will live at least as long as lifetime 'a
// * lifetime of the returned reference is the same as the smaller of the lifetimes of the
//   references passed in
fn longest<'a>(x: &'a str, y: &'a str) -> &'a str {
    if x.len() > y.len() {
        x
    } else {
        y
    }
}

/*
fn failure() {
    let string1 = String::from("long string is long");
    let result;
    {
        let string2 = String::from("xyz");
        result = longest(string1.as_str(), string2.as_str());
    }
    println!("The longest string is {}", result);

}
*/

// lifetime in struct
// * an instance can't outlive the reference it holds in 'part'
struct ImportantExcerpt<'a> {
    part: &'a str,
}

fn struct_lifetime() {
    let novel = String::from("Call me Ishmael. Some years ago...");
    let first_sentence = novel.split('.')
        .next()
        .expect("Could not find a '.'");

    let _i = ImportantExcerpt { part: first_sentence };
}

// lifetime in method
impl<'a> ImportantExcerpt<'a> {
    fn level(&self) -> i32 {
        3
    }

    fn announce_and_return_part(&self, announcement: &str) -> &str {
        println!("Attention please: {}", announcement);
        self.part
    }
}

// generic type, trait bound and lifetime together
use std::fmt::Display;

fn longest_with_an_announcement<'a, T>(x: &'a str, y: &'a str, ann: T) -> &'a str
    where T: Display
{
    println!("Announcement! {}", ann);
    if x.len() > y.len() {
        x
    } else {
        y
    }
}

fn main() {
    println!("The longest string is {}",
             longest("abcd", "xyz"));
    struct_lifetime();
}
