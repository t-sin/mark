use std::iter::Peekable;
use std::str::Chars;
use std::sync::{Arc, Mutex};

#[derive(Debug)]
enum Sexp {
    Null,
    Nil,
    Int(i64),
    Char(char),
    Str(String),
    // Symbol(),
    // Timestamp(),
    Cons(Arc<Mutex<Sexp>>, Arc<Mutex<Sexp>>),
}

fn cheap_print(c: Arc<Mutex<Sexp>>) {
    match &*c.lock().unwrap() {
        Sexp::Null => print!("NULL"),
        Sexp::Nil => print!("()"),
        Sexp::Int(i) => print!("{}", i),
        Sexp::Char(c) => print!("{:?}", c),
        Sexp::Str(s) => print!("\"{}\"", s),
        Sexp::Cons(car, cdr) => {
            print!("(");
            cheap_print(car.clone());
            print!(" . ");
            cheap_print(cdr.clone());
            print!(")");
        },
    };
}


fn is_whitespace(c: char) -> bool {
    c == ' ' || c == '\n' || c == '\t'
}

fn is_delimiter(c: char) -> bool {
    is_whitespace(c) || c == ')'
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
            },
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

fn cheap_read_string(chars: &mut Peekable<Chars>) -> Sexp {
    Sexp::Nil
}

fn cheap_read_char(chars: &mut Peekable<Chars>) -> Sexp {
    Sexp::Nil
}

fn cheap_read_int(chars: &mut Peekable<Chars>) -> Sexp {
    Sexp::Nil
}

fn cheap_read_symbol(chars: &mut Peekable<Chars>) -> Sexp {
    Sexp::Nil
}

fn cheap_read_list(chars: &mut Peekable<Chars>) -> Sexp {
    Sexp::Nil
}

fn cheap_read_1(chars: &mut Peekable<Chars>) -> Sexp {
    skip_whitespaces(chars);
    let c = chars.peek();
    match c {
        None => Sexp::Nil,
        Some(';') => {
            skip_comment(chars);
            Sexp::Nil
        },
        Some(')') => panic!("unexpected close paren"),
        Some('(') => cheap_read_list(chars),
        Some('\"') => cheap_read_string(chars),
        Some('#') => cheap_read_char(chars),
        Some(c) if c.is_digit(10) => cheap_read_int(chars),
        Some(c) => cheap_read_symbol(chars),
    }
}

fn cheap_read(s: String) -> Vec<Arc<Mutex<Sexp>>> {
    let mut chars = s.chars().peekable();
    let mut values = Vec::new();
    values
}


fn main() {
    let code = "12345 #\\1 () (1) \"hoge fuga\" (1 (2 (10 20 30) (3 4) 5)) 123".to_string();
    println!("code: {:?}", code);
    let cons_vec = cheap_read(code);
    for cons in cons_vec {
        cheap_print(cons);
        println!("");
    }
}
