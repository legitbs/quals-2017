#![feature(alloc_system)]
extern crate alloc_system;

use std::io::{Write, stdout, stdin};
mod check;
use check::check;

fn main() {
    println!("enter code:");
    stdout().flush().unwrap();

    let mut buffer = String::new();
    let _bytes_read = stdin().read_line(&mut buffer).unwrap();

    let sum = check(buffer);

    println!("sum is {}", sum);
}
