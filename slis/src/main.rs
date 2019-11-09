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
    Symbol(String),
    // Timestamp(),
    Cons(Arc<Mutex<Sexp>>, Arc<Mutex<Sexp>>),
}

impl Sexp {
    fn new(sexp: Sexp) -> Arc<Mutex<Sexp>> {
        Arc::new(Mutex::new(sexp))
    }
}

fn cheap_print_elem(sexp: &Sexp) {
   match sexp {
        Sexp::Null => print!("NULL"),
        Sexp::Nil => print!("()"),
        Sexp::Int(i) => print!("{}", i),
        Sexp::Char(c) => print!("{:?}", c),
        Sexp::Str(s) => print!("\"{}\"", s),
        Sexp::Symbol(n) => print!("{}", n),
        Sexp::Cons(car, cdr) => {
            print!("(");
            cheap_print_list(car.clone(), cdr.clone());
            print!(")");
        },
    };
}

fn cheap_print_list(car: Arc<Mutex<Sexp>>, cdr: Arc<Mutex<Sexp>>) {
    cheap_print(car.clone());
    match &*cdr.lock().unwrap() {
        Sexp::Cons(ref car, ref cdr) => {
            print!(" ");
            cheap_print_list(car.clone(), cdr.clone());
        },
        Sexp::Nil => (),
        ref c => {
            print!(" . ");
            cheap_print_elem(c);
        },
    }
}

fn cheap_print(c: Arc<Mutex<Sexp>>) {
    cheap_print_elem(&*c.lock().unwrap());
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

fn cheap_read_string(chars: &mut Peekable<Chars>) -> Arc<Mutex<Sexp>> {
    chars.next();
    let mut s = String::new();
    loop {
        match chars.peek() {
            Some('"') => {
                chars.next();
                break;
            },
            None => break,
            Some(c) => s.push(chars.next().unwrap()),
        }
    }
    Sexp::new(Sexp::Str(s))
}

fn cheap_read_char(chars: &mut Peekable<Chars>) -> Arc<Mutex<Sexp>> {
    chars.next();
    if let Some('\\') = chars.peek() {
        chars.next();
        let mut name = String::new();
        loop {
            if let Some(c)  = chars.peek() {
                if !is_delimiter(*c) {
                    name.push(chars.next().unwrap());
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        match &name[..] {
            "space" => Sexp::new(Sexp::Char(' ')),
            "tab" => Sexp::new(Sexp::Char('\t')),
            "newline" => Sexp::new(Sexp::Char('\n')),
            c => {
                if let Some(c) = name.chars().nth(0) {
                    Sexp::new(Sexp::Char(c))
                } else {
                    panic!("invalid character literal (missing char itself)")
                }
            }
        }
    } else {
        panic!("invalid character literal");
    }
}

fn cheap_read_int(chars: &mut Peekable<Chars>) -> Arc<Mutex<Sexp>> {
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
        Sexp::new(Sexp::Int(n))
    } else {
        panic!("cannot parse {:?} as i64", s)
    }
}

fn cheap_read_symbol(chars: &mut Peekable<Chars>) -> Arc<Mutex<Sexp>> {
    let mut s = String::new();
    loop {
        match chars.peek() {
            Some(c) if !is_delimiter(*c) => s.push(chars.next().unwrap()),
            _ => break,
        }
    }
    Sexp::new(Sexp::Symbol(s))
}

fn vec_to_cons(vec: &[Arc<Mutex<Sexp>>]) -> Arc<Mutex<Sexp>> {
    if vec.len() == 0 {
        Sexp::new(Sexp::Nil)
    } else {
        Sexp::new(Sexp::Cons(vec[0].clone(), vec_to_cons(&vec[1..])))
    }
}

fn cheap_read_list(chars: &mut Peekable<Chars>) -> Arc<Mutex<Sexp>> {
    chars.next();
    let mut vec = Vec::new();
    loop {
        skip_whitespaces(chars);
        match chars.peek() {
            Some(')') => {
                chars.next();
                break;
            },
            None => break,
            _ => vec.push(cheap_read(chars)),
        }
    }
    vec_to_cons(&vec[..])
}

fn cheap_read(chars: &mut Peekable<Chars>) -> Arc<Mutex<Sexp>> {
    skip_whitespaces(chars);
    let c = chars.peek();
    match c {
        None => Sexp::new(Sexp::Nil),
        Some(';') => {
            skip_comment(chars);
            Sexp::new(Sexp::Nil)
        },
        Some(')') => panic!("unexpected close paren"),
        Some('(') => cheap_read_list(chars),
        Some('\"') => cheap_read_string(chars),
        Some('#') => cheap_read_char(chars),
        Some(c) if c.is_digit(10) => cheap_read_int(chars),
        Some(c) => cheap_read_symbol(chars),
    }
}

fn cheap_read_all(s: String) -> Vec<Arc<Mutex<Sexp>>> {
    let mut chars = s.chars().peekable();
    let mut values = Vec::new();
    loop {
        let sexp = cheap_read(&mut chars);
        values.push(sexp);
        if let None = chars.peek() {
            break;
        }
    }
    values
}

fn eval(sexp: Arc<Mutex<Sexp>>) -> Arc<Mutex<Sexp>> {
    match *sexp.lock().unwrap() {
        Sexp::Null => panic!("whoa! it's null!"),
        Sexp::Nil => sexp.clone(),
        Sexp::Int(_) => sexp.clone(),
        Sexp::Char(_) => sexp.clone(),
        Sexp::Str(_) => sexp.clone(),
        Sexp::Symbol(ref name) => {
            println!("{:?} is symbol!", name);
            sexp.clone()
        },
        Sexp::Cons(ref car, ref cdr) => {
            sexp.clone()
        },
    }
}

fn main() {
    let code = "12345 #\\1 () (1) \"hoge fuga\" (1 (2 (10 20 30) (3 4) 5)) 123 symbol :keyword".to_string();
    println!("code: {:?}", code);
    let sexp_vec = cheap_read_all(code);
    for sexp in sexp_vec {
        cheap_print(eval(sexp));
        println!("");
    }
}
