use object_oriented::Draw;
use object_oriented::{Screen, Button};

struct SelectBox {
    width: u32,
    height: u32,
    options: Vec<String>,
}

impl Draw for SelectBox {
    fn draw(&self) {
        //println!("SelectBox: {:?}", self.options);
        println!("SelectBox: ");
        for opt in self.options.iter() {
            println!("\t{}", opt);
        }
    }
}

fn test_screen() {
    let screen = Screen {
        components: vec![
            Box::new(
                SelectBox {
                    width: 75,
                    height: 10,
                    options: vec![
                        String::from("Yes"),
                        String::from("Maybe"),
                        String::from("No")
                    ],
                }
            ),
            Box::new(
                Button {
                    width: 50,
                    height: 10,
                    label: String::from("OK"),
                }
            ),
        ],
    };

    screen.run();
}

// state
use object_oriented::Post;

fn test_post() {
    let mut post = Post::new();

    post.add_text("I ate a salad for lunch today");
    assert_eq!("", post.content());

    post.request_review();
    assert_eq!("", post.content());

    post.approve();
    assert_eq!("I ate a salad for lunch today", post.content());

    println!("{}", post.content());
}

fn main() {
    test_screen();
    test_post();
}
