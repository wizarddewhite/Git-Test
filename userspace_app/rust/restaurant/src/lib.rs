mod front_of_house;
use crate::front_of_house::hosting;
use crate::front_of_house::serving;

mod back_of_house;
use crate::back_of_house::Breakfast;

#[cfg(feature = "tastes")]
mod tastes;

pub fn eat_at_restaurant() {
    hosting::add_to_waitlist();
    hosting::seat_at_table();
    serving::take_over();
    serving::serve_order();
    serving::take_payment();

    // Order a breakfast in the summer with Rye toast
    let mut meal = Breakfast::summer("Rye");
    println!("I want {} toast please", meal.toast);
    // Change our mind about what bread we'd like
    meal.toast = String::from("Wheat");
    println!("Hmm...I'd like {} toast please", meal.toast);

    // The next line won't compile if we uncomment it; we're not allowed
    // to see or modify the seasonal fruit that comes with the meal
    // meal.seasonal_fruit = String::from("blueberries");

    tastes::like();
}
