use std::rc::Rc;


#[derive(Debug)]
enum Atom {
    Nil,
    Int(i64),
    Char(char),
    Str(String),
    // Symbol(),
    // Timestamp(),
}

#[derive(Debug)]
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
    initial: bool,
    input: String,
    pos: usize,
    ast: Cons,
    current_node: Cons,
    stack: Vec<Token>,
}

fn stack_top(state: &ReadState) -> Option<usize> {
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

    } else if c == '#' {
        let mut char_token = Token {
            kind: TokenKind::Char,
            buffer: "".to_string(),
        };
        char_token.buffer.push(c);
        state.stack.push(char_token);

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
        TokenKind::Char => {
            state.ast = Cons::Atom(Atom::Char(token.buffer.chars().nth(2).unwrap()));
        },
        _ => (),
    }
}

fn cheap_read_non_terminate(state: &mut ReadState) {
    let c = state.input.chars().nth(state.pos).unwrap();
    println!("stack: {:?}", state.stack);
    println!("char is {:?}!", c);

    match stack_top(state) {
        Some(pos) => match state.stack[pos].kind {
            TokenKind::Initial => {
                cheap_read_dispatch(state);
            },
            TokenKind::Integer => {
                if is_delimiter(c) {
                    cheap_read_terminate(state);
                } else if c.is_digit(10) {
                    state.stack[pos].buffer.push(c);
                    state.pos += 1;
                } else {
                    panic!("{} is not allowed in an integer", c);
                }
            },
            TokenKind::Char => {
                if state.stack[pos].buffer.len() == 1 && c == '\\' {
                    state.stack[pos].buffer.push(c);
                    state.pos += 1;
                } else if state.stack[pos].buffer.len() == 2 {
                    state.stack[pos].buffer.push(c);
                    state.pos += 1;
                    cheap_read_terminate(state);
                } else {
                    panic!("{} is not allowed in character literal", c);
                }
            }
            _ => (),
        },
        None => panic!("...unmatched what??"),
    }
}

fn cheap_read_1(state: &mut ReadState) {
    cheap_read_non_terminate(state);
    loop {
        if state.pos >= state.input.len() {
            cheap_read_terminate(state);
            if let Some(top) = stack_top(state) {
                if top == 0 {
                    if let TokenKind::Initial = state.stack[top].kind {
                        state.stack.pop();
                        state.stack.push(Token { kind: TokenKind::EOF, buffer: "".to_string() });
                        return;
                    }
                }
            } else {
                println!("stack broken");
            }
            return;
        }

        if state.initial == true {
            state.initial = false;
        } else {
            if let Some(top) = stack_top(state) {
                if top == 0 {
                    if let TokenKind::Initial = state.stack[top].kind {
                        return;
                    }
                }
            } else {
                println!("stack broken");
            }
        }

        cheap_read_non_terminate(state);
    }
}

fn cheap_read(s: String) -> Vec<Cons> {
    let mut cons_vec = Vec::new();
    let mut state = ReadState {
        initial: true, input: s, pos: 0,
        ast: Cons::Null,
        current_node: Cons::Null,
        stack: vec![Token { kind: TokenKind::Initial, buffer: "".to_string()}],
    };
    loop {
        cheap_read_1(&mut state);
        cons_vec.push(state.ast);
        println!("stack: {:?}", state.stack);
        println!("cons_vec: {:?}", cons_vec);

        state.initial = true;
        state.ast = Cons::Null;
        state.current_node = Cons::Null;

        if let Some(top) = stack_top(&state) {
            if let TokenKind::EOF = state.stack[top].kind {
                break;
            }
        }
    };
    cons_vec
}


fn main() {
    let code = "12345 #\\1 123".to_string();
    println!("code: {:?}", code);
    let cons_vec = cheap_read(code);
    for cons in cons_vec {
        cheap_print(&cons);
        println!("");
    }
}
