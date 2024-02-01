pub trait Summary {
    fn summarize_author(&self) -> String;

    // default implementation
    fn summarize(&self) -> String {
        format!("(Read more from {}...)", self.summarize_author())
    }
}

pub struct NewsArticle {
    pub headline: String,
    pub location: String,
    pub author: String,
    pub content: String,
}

impl Summary for NewsArticle {
    fn summarize_author(&self) -> String {
        format!("@{}", self.author)
    }
    // use default summarize()
}

pub struct Tweet {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,
}

impl Summary for Tweet {
    fn summarize_author(&self) -> String {
        format!("@{}", self.username)
    }
    fn summarize(&self) -> String {
        format!("{}, {}", self.username, self.content)
    } 
}

// function on trait
pub fn notify(item: impl Summary) {
    println!("Breaking news! {}", item.summarize());
}

// trait bound syntax, the same as notify()
pub fn notify2<T: Summary>(item: T) {
    println!("(trait bound)Breaking news! {}", item.summarize());
}

pub trait Display {
    fn show(&self) -> String;
}

// use trait bound for conditional method
impl<T: Summary> Display for T {
    fn show(&self) -> String {
        format!("{}", self.summarize())
    }
}

// inheritance
pub trait Publish: Display {
    fn publish(&self) -> String {
        format!("Publish: {}", self.show())
    }
}

impl<T: Summary> Publish for T {
}

fn main() {
    let tweet = Tweet {
        username: String::from("horse_ebooks"),
        content: String::from("of course, as you probably already know, people"),
        reply: false,
        retweet: false,
    };

    println!("1 new tweet: {}", tweet.summarize());

    let news = NewsArticle {
        headline: String::from("Penguins win the Stanley Cup Championship!"),
        location: String::from("Pittsburgh, PA, USA"),
        author: String::from("Iceburgh"),
        content: String::from("The Pittsburgh Penguins once again are the best
                              hockey team in the NHL."),
    };

    println!("1 new news: {}", news.summarize());

    println!("Display show: {}", news.show());
    println!("Publish news: {}", news.publish());

    notify(tweet);
    notify2(news);
    // println!("1 new tweet: {}", tweet.summarize()); tweet is moved to notify

}