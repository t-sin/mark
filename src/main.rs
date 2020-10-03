#[derive(Debug, Clone, PartialEq)]
pub enum Op {
    Nop,
}

pub trait Machine {
    fn fetch(&self) -> Op;
    fn dump(&self);
    fn interpret_1(&self);
}

pub trait MemoryMap {}

pub struct Memory {}

// type Stack = Vec<Op>;

#[derive(Debug, Clone)]
pub struct VM {}

fn main() {
    println!("Hello, world!");
}
