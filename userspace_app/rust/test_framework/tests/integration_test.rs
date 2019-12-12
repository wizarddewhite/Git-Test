use test_framework;

// cargo test --test integration_test
#[test]
fn it_adds_two() {
    assert_eq!(4, test_framework::add_two(2));
}
