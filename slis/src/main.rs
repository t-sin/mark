use std::rc::Rc;


enum Atom {
    Nil,
    Int(i64),
    Char(char),
    Str(String),
    // Symbol(),
    // Timestamp(),
}

enum Cons {
    Null,
    Atom(Atom),
    Cons(Rc<Cons>, Rc<Cons>),
}


fn cheap_print(c: &Cons) {
    match c {
        Cons::Null => print!("NULL"),
        Cons::Atom(a) => match a {
            Atom::Nil => print!("()"),
            Atom::Int(i) => print!("{}", i),
            Atom::Char(c) => print!("{:?}", c),
            Atom::Str(s) => print!("\"{}\"", s),
        },
        Cons::Cons(car, cdr) => {
            print!("(");
            cheap_print(car);
            print!(" . ");
            cheap_print(cdr);
            print!(")");
        },
    };
}


#[derive(Debug)]
enum TokenKind {
    Initial,
    EOF,
    Space,
    OpenParen,
    CloseParen,
    Integer,
    StringDelimiter,
    String,
    Char,
}

#[derive(Debug)]
struct Token {
    kind: TokenKind,
    buffer: String,
}

struct ReadState {
    input: String,
    pos: usize,
    ast: Vec<Cons>,
    current_node: Cons,
    stack: Vec<Token>,
}

fn cheap_read_int(state: &mut ReadState) {}
fn cheap_read_str(state: &mut ReadState) {}
fn cheap_read_char(state: &mut ReadState) {}
fn cheap_read_list(state: &mut ReadState) {}
fn cheap_read_symbol(state: &mut ReadState) {}

fn top(state: &ReadState) -> Option<&Token> {
    if state.stack.len() < 1 {
        None
    } else {
        Some(&state.stack[state.stack.len() - 1])
    }
}

fn cheap_read_dispatch(state: &mut ReadState) {
    let c = state.input.chars().nth(state.pos).unwrap();

    if c == ' ' || c == '\n' {
        state.pos += 1;

    } else if c.is_digit(10) {
        let mut int_token = Token {
            kind: TokenKind::Integer, buffer: "".to_string()
        };
        int_token.buffer.push(c);
        state.stack.push(int_token);

    } else if c == '\"' {
        cheap_read_str(state);
    } else if c == '(' {
        state.pos += 1;
        cheap_read_list(state);
    } else {
        cheap_read_symbol(state);
    }
    state.pos += 1;
 }

fn cheap_read_1(state: &mut ReadState) {
    loop {
        if state.pos >= state.input.len() {
            println!("EOF!!");
            return;
        }
        let c = state.input.chars().nth(state.pos).unwrap();
        println!("stack: {:?}", state.stack);
        println!("char is {:?}!", c);

        match top(state) {
            Some(token) => match token.kind {
                TokenKind::Initial => cheap_read_dispatch(state),
                TokenKind::Integer => (),
                _ => (),
            },
            None => panic!("????"),
        };
    }
}

fn cheap_read(s: String) -> Vec<Cons> {
    let mut state = ReadState {
        input: s, pos: 0,
        ast: Vec::new(),
        current_node: Cons::Null,
        stack: vec![Token { kind: TokenKind::Initial, buffer: "".to_string()}],
    };
    cheap_read_1(&mut state);
    println!("stack: {:?}", state.stack);
    state.ast
}


fn main() {
    let code = "12345".to_string();
    for sexp in cheap_read(code) {
        cheap_print(&sexp);
        println!("");
    }
}
