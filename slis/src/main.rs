use std::cmp::PartialEq;
use std::iter::Peekable;
use std::str::Chars;
use std::sync::{Arc, Mutex};

#[derive(Debug)]
enum Sexp<'a> {
    None,
    Nil,
    Int(i64),
    Char(char),
    Str(String),
    Symbol(String),
    // Timestamp(),
    Cons(&'a Cell<'a>, &'a Cell<'a>),
}

#[derive(Debug)]
struct Cell<'a> {
    id: usize,
    live: bool,
    val: Sexp<'a>,
}

impl<'a> PartialEq for Sexp<'a> {
    fn eq(&self, other: &Self) -> bool {
        match self {
            Sexp::None => {
                if let Sexp::None = other {
                    true
                } else {
                    false
                }
            }
            Sexp::Nil => {
                if let Sexp::Nil = other {
                    true
                } else {
                    false
                }
            }
            Sexp::Int(s) => {
                if let Sexp::Int(o) = other {
                    s == o
                } else {
                    false
                }
            }
            Sexp::Char(s) => {
                if let Sexp::Char(o) = other {
                    s == o
                } else {
                    false
                }
            }
            Sexp::Str(s) => {
                if let Sexp::Str(o) = other {
                    &s[..] == &o[..]
                } else {
                    false
                }
            }
            Sexp::Symbol(s) => {
                if let Sexp::Symbol(o) = other {
                    &s[..] == &o[..]
                } else {
                    false
                }
            }
            _ => false,
        }
    }
}

struct Memory<'a> {
    cells: Vec<Cell<'a>>,
}

impl<'a> Memory<'a> {
    pub fn new(size: usize) -> Memory<'a> {
        let mut cells = Vec::new();
        let mut count = 0;
        cells.push(Cell {
            id: count,
            live: true,
            val: Sexp::Nil,
        });
        for _ in 1..size {
            cells.push(Cell {
                id: count,
                live: false,
                val: Sexp::None,
            });
            count += 1;
        }
        Memory { cells: cells }
    }

    pub fn get_mut(&mut self, i: usize) -> Option<&mut Cell<'a>> {
        self.cells.get_mut(i)
    }

    pub fn get_new(&mut self) -> Option<&mut Cell<'a>> {
        for c in self.cells.iter_mut() {
            if c.live == false {
                c.live = true;
                return Some(c);
            }
        }
        None
    }
}

fn cheap_print_list<'a>(car: &Cell<'a>, cdr: &Cell<'a>) {
    cheap_print(car);
    match cdr.val {
        Sexp::Cons(ref car, ref cdr) => {
            print!(" ");
            cheap_print_list(car, cdr);
        }
        Sexp::Nil => (),
        _ => {
            print!(" . ");
            cheap_print(cdr);
        }
    }
}

fn cheap_print<'a>(cell: &Cell<'a>) {
    match &cell.val {
        Sexp::None => print!("None"),
        Sexp::Nil => print!("()"),
        Sexp::Int(i) => print!("{}", i),
        Sexp::Char(c) => print!("{:?}", c),
        Sexp::Str(s) => print!("\"{}\"", s),
        Sexp::Symbol(n) => print!("{}", n),
        Sexp::Cons(ref car, ref cdr) => {
            print!("(");
            cheap_print_list(car, cdr);
            print!(")");
        }
    };
}

fn is_whitespace(c: char) -> bool {
    c == ' ' || c == '\n' || c == '\t'
}

fn is_delimiter(c: char) -> bool {
    is_whitespace(c) || c == ')' || c == '('
}

fn skip_whitespaces(chars: &mut Peekable<Chars>) {
    loop {
        let c = chars.peek();
        match c {
            Some(c) => {
                if is_whitespace(*c) {
                    chars.next();
                } else {
                    break;
                }
            }
            _ => break,
        };
    }
}

fn skip_comment(chars: &mut Peekable<Chars>) {
    loop {
        let c = chars.peek();
        match c {
            Some('\n') => break,
            None => break,
            _ => chars.next(),
        };
    }
}

fn cheap_read_string<'a>(chars: &mut Peekable<Chars>, mem: &'a mut Memory<'a>) -> &'a Cell<'a> {
    chars.next();
    let mut s = String::new();
    loop {
        match chars.peek() {
            Some('"') => {
                chars.next();
                break;
            }
            None => break,
            Some(_) => s.push(chars.next().unwrap()),
        }
    }

    let cell = mem.get_new();
    if let Some(cell) = cell {
        cell.val = Sexp::Str(s);
        cell
    } else {
        panic!("cell pool exhausted");
    }
}

fn cheap_read_char<'a>(chars: &mut Peekable<Chars>, mem: &'a mut Memory<'a>) -> &'a Cell<'a> {
    chars.next();
    if let Some('\\') = chars.peek() {
        chars.next();
        let mut name = String::new();
        loop {
            if let Some(c) = chars.peek() {
                if !is_delimiter(*c) {
                    name.push(chars.next().unwrap());
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        let ch = match &name[..] {
            "space" => Sexp::Char(' '),
            "tab" => Sexp::Char('\t'),
            "newline" => Sexp::Char('\n'),
            _ => {
                if let Some(c) = name.chars().nth(0) {
                    Sexp::Char(c)
                } else {
                    panic!("invalid character literal (missing char itself)")
                }
            }
        };
        let cell = mem.get_new();
        if let Some(cell) = cell {
            cell.val = ch;
            cell
        } else {
            panic!("cell pool exhausted");
        }
    } else {
        panic!("invalid character literal");
    }
}

fn cheap_read_int<'a>(chars: &mut Peekable<Chars>, mem: &'a mut Memory<'a>) -> &'a Cell<'a> {
    let mut s = String::new();
    s.push(chars.next().unwrap());
    loop {
        if let Some(c) = chars.peek() {
            if c.is_digit(10) {
                s.push(chars.next().unwrap());
            } else {
                break;
            }
        } else {
            break;
        }
    }
    if let Ok(n) = s.parse::<i64>() {
        let cell = mem.get_new();
        if let Some(cell) = cell {
            cell.val = Sexp::Int(n);
            cell
        } else {
            panic!("cell pool exhausted");
        }
    } else {
        panic!("cannot parse {:?} as i64", s)
    }
}

fn cheap_read_symbol<'a>(chars: &mut Peekable<Chars>, mem: &'a mut Memory<'a>) -> &'a Cell<'a> {
    let mut s = String::new();
    loop {
        match chars.peek() {
            Some(c) if !is_delimiter(*c) => s.push(chars.next().unwrap()),
            _ => break,
        }
    }
    let cell = mem.get_new();
    if let Some(cell) = cell {
        cell.val = Sexp::Symbol(s);
        cell
    } else {
        panic!("cell pool exhausted");
    }
}

fn vec_to_cons<'a>(vec: &[&'a Cell<'a>], mem: &'a mut Memory<'a>) -> &'a Cell<'a> {
    let cell = mem.get_new();
    if let Some(cell) = cell {
        if vec.len() == 0 {
            cell.val = Sexp::Nil;
            cell
        } else {
            cell.val = Sexp::Cons(vec[0], vec_to_cons(&vec[1..], mem));
            cell
        }
    } else {
        panic!("cell pool exhausted");
    }
}

fn cheap_read_list<'a>(chars: &mut Peekable<Chars>, mem: &'a mut Memory<'a>) -> &'a Cell<'a> {
    chars.next();
    let mut vec = Vec::new();
    loop {
        skip_whitespaces(chars);
        match chars.peek() {
            Some(')') => {
                chars.next();
                break;
            }
            None => break,
            _ => vec.push(cheap_read(chars, mem)),
        }
    }
    vec_to_cons(&vec[..], mem)
}

fn cheap_read<'a>(chars: &mut Peekable<Chars>, mem: &'a mut Memory<'a>) -> &'a Cell<'a> {
    skip_whitespaces(chars);
    let c = chars.peek();
    match c {
        None => {
            let cell = mem.get_new();
            if let Some(cell) = cell {
                cell.val = Sexp::Nil;
                cell
            } else {
                panic!("cell pool exhausted");
            }
        }
        Some(';') => {
            skip_comment(chars);
            let cell = mem.get_new();
            if let Some(cell) = cell {
                cell.val = Sexp::Nil;
                cell
            } else {
                panic!("cell pool exhausted");
            }
        }
        Some(')') => panic!("unexpected close paren"),
        Some('(') => cheap_read_list(chars, mem),
        Some('\"') => cheap_read_string(chars, mem),
        Some('#') => cheap_read_char(chars, mem),
        Some(c) if c.is_digit(10) => cheap_read_int(chars, mem),
        Some(_) => cheap_read_symbol(chars, mem),
    }
}

fn cheap_read_all<'a>(s: String, mem: &'a mut Memory<'a>) -> Vec<&'a Cell<'a>> {
    let mut chars = s.chars().peekable();
    let mut values = Vec::new();
    loop {
        let sexp = cheap_read(&mut chars, mem);
        values.push(sexp);
        if let None = chars.peek() {
            break;
        }
    }
    values
}

fn eval<'a>(cell: &'a Cell<'a>, mem: &'a mut Memory<'a>) -> &'a Cell<'a> {
    match cell.val {
        Sexp::None => panic!("whoa! it's none!"),
        Sexp::Nil => cell,
        Sexp::Int(_) => cell,
        Sexp::Char(_) => cell,
        Sexp::Str(_) => cell,
        Sexp::Symbol(ref name) => {
            println!("{:?} is symbol!", name);
            cell
        }
        Sexp::Cons(ref car, ref cdr) => match &car.val {
            Sexp::Symbol(ref s) if s == "quote" => cdr,
            Sexp::Symbol(ref s) if s == "cond" => cdr,
            Sexp::Symbol(ref s) if s == "defun" => cdr,
            Sexp::Symbol(ref s) if s == "setq" => cdr,
            Sexp::Symbol(ref s) if s == "lambda" => cdr,
            Sexp::Symbol(_name) => {
                let cell = mem.get_new();
                if let Some(cell) = cell {
                    cell.val = Sexp::Nil;
                    cell
                } else {
                    panic!("cell pool exhausted");
                }
            }
            _ => {
                println!("it should raise 'invalid function call'");
                let cell = mem.get_new();
                if let Some(cell) = cell {
                    cell.val = Sexp::None;
                    cell
                } else {
                    panic!("cell pool exhausted");
                }
            }
        },
    }
}

fn main() {
    let mut mem = Memory::new(1000);
    let code = "12345 #\\1 () (1) \"hoge fuga\" (1 (2 (10 20 30) (3 4) 5)) 123 symbol :keyword"
        .to_string();
    println!("code: {:?}", code);
    let sexp_vec = cheap_read_all(code, &mut mem);
    for sexp in sexp_vec {
        cheap_print(eval(sexp, &mut mem));
        println!("");
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn test_cheap_read(code: &str, expected: Sexp) {
        let str = code.to_string();
        let mut peekable = str.chars().peekable();
        assert_eq!(*cheap_read(&mut peekable).lock().unwrap(), expected);
    }

    #[test]
    fn cheap_reader() {
        let code = "12345"; // integer
        test_cheap_read(code, Sexp::Int(12345));
        let code = "#\\1"; // charactor
        test_cheap_read(code, Sexp::Char('1'));

        // lists
        let code = "()";
        test_cheap_read(code, Sexp::Nil);
        let code = "(1)";
        let expected = Sexp::Cons(
            Arc::new(Mutex::new(Sexp::Int(1))),
            Arc::new(Mutex::new(Sexp::Nil)),
        );
        test_cheap_read(code, expected);
        p
    }
}
