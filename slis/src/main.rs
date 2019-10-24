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
    ast: Cons,
    current_node: Cons,
    stack: Vec<Token>,
}

fn top(state: &ReadState) -> Option<usize> {
    if state.stack.len() < 1 {
        None
    } else {
        Some(state.stack.len() - 1)
    }
}

fn is_delimiter(c: char) -> bool {
    c == ' ' || c == '\n'
}

fn cheap_read_dispatch(state: &mut ReadState) {
    let c = state.input.chars().nth(state.pos).unwrap();

    if is_delimiter(c) {
        state.pos += 1;

    } else if c.is_digit(10) {
        let mut int_token = Token {
            kind: TokenKind::Integer, buffer: "".to_string()
        };
        int_token.buffer.push(c);
        state.stack.push(int_token);

    } else if c == '\"' {
        // read string
    } else if c == '(' {
        state.pos += 1;
        // read list
    } else {
        // read symbol
    }
    state.pos += 1;
 }

fn cheap_read_terminate(state: &mut ReadState) {
    let token = state.stack.pop().unwrap();
    match token.kind {
        TokenKind::Integer => {
            if let Ok(i) = token.buffer.parse::<i64>() {
                state.ast = Cons::Atom(Atom::Int(i));
                state.pos += 1;
            } else {
                panic!("'{}' is not a number", token.buffer);
            }
        },
        _ => (),
    }
}

fn cheap_read_non_terminate(state: &mut ReadState) {
    let c = state.input.chars().nth(state.pos).unwrap();
    println!("stack: {:?}", state.stack);
    println!("char is {:?}!", c);

    if is_delimiter(c) {
        cheap_read_terminate(state)
    } else {
        match top(state) {
            Some(pos) => match state.stack[pos].kind {
                TokenKind::Initial => cheap_read_dispatch(state),
                TokenKind::Integer => {
                    if c.is_digit(10) {
                        state.stack[pos].buffer.push(c);
                        state.pos += 1;
                    }
                },
                _ => (),
            },
            None => panic!("????"),
        };
    }
}

fn cheap_read_1(state: &mut ReadState) {
    loop {
        if state.pos >= state.input.len() {
            println!("EOF!!");
            if state.stack.len() > 1 {
                cheap_read_terminate(state);
            }
            return;
        }
        cheap_read_non_terminate(state);
    }
}

fn cheap_read(s: String) -> Cons {
    let mut state = ReadState {
        input: s, pos: 0,
        ast: Cons::Null,
        current_node: Cons::Null,
        stack: vec![Token { kind: TokenKind::Initial, buffer: "".to_string()}],
    };
    cheap_read_1(&mut state);
    println!("stack: {:?}", state.stack);
    state.ast
}


fn main() {
    let code = "12345".to_string();
    cheap_print(&cheap_read(code));
    println!("");
}
