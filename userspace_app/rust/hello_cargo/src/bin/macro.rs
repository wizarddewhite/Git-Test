#[macro_export]
macro_rules! my_vec {
    ( $( $x:expr ),* ) => {
        {
            let mut temp_vec = Vec::new();
            $(
                temp_vec.push($x);
            )*
            temp_vec
        }
    };
}

macro_rules! hashmap {
    ($( $key: expr => $val: expr ),*) => {{
         let mut map = ::std::collections::HashMap::new();
         $( map.insert($key, $val); )*
         map
    }}
}

// cargo expand --bin macro
// would expand the definition, which is helpful for understanding the code

fn main() {
    let v: Vec<u32> = my_vec![1, 2, 3];
    println!("Test on Macro");
    println!("My vec: {:?}", v);

    let counts = hashmap!['A' => 0, 'C' => 0, 'G' => 0, 'T' => 0];
    println!("{:?}", counts);
}
