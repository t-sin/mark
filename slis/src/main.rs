use std::sync::{Arc, Mutex};


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
    Atom(Arc<Mutex<Atom>>),
    Cons(Arc<Mutex<Cons>>, Arc<Mutex<Cons>>),
}


fn cheap_print(c: Arc<Mutex<Cons>>) {
    match &*c.lock().unwrap() {
        Cons::Null => print!("NULL"),
        Cons::Atom(a) => match &*a.lock().unwrap() {
            Atom::Nil => print!("()"),
            Atom::Int(i) => print!("{}", i),
            Atom::Char(c) => print!("{:?}", c),
            Atom::Str(s) => print!("\"{}\"", s),
        },
        Cons::Cons(car, cdr) => {
            print!("(");
            cheap_print(car.clone());
            print!(" . ");
            cheap_print(cdr.clone());
            print!(")");
        },
    };
}


#[derive(Debug)]
enum ObjectType {
    Initial,
    EOF,
    List,
    Integer,
    String,
    Char,
}

#[derive(Debug)]
struct ParseState {
    kind: ObjectType,
    buffer: String,
}

struct Reader {
    initial: bool,
    input: String,
    pos: usize,
    ast: Arc<Mutex<Cons>>,
    current_cons: Arc<Mutex<Cons>>,
    stack: Vec<ParseState>,
}

fn stack_top(state: &Reader) -> Option<usize> {
    if state.stack.len() < 1 {
        None
    } else {
        Some(state.stack.len() - 1)
    }
}

fn is_whitespace(c: char) -> bool {
    c == ' ' || c == '\n' || c == '\t'
}

fn is_delimiter(c: char) -> bool {
    is_whitespace(c) || c == ')'
}

fn cheap_read_dispatch(state: &mut Reader) {
    let c = state.input.chars().nth(state.pos).unwrap();

    if is_whitespace(c) {
        state.pos += 1;

    } else if c.is_digit(10) {
        state.pos += 1;
        let mut ps = ParseState {
            kind: ObjectType::Integer, buffer: "".to_string()
        };
        ps.buffer.push(c);
        state.stack.push(ps);


    } else if c == '#' {
        state.pos += 1;
        let mut ps = ParseState {
            kind: ObjectType::Char,
            buffer: "".to_string(),
        };
        ps.buffer.push(c);
        state.stack.push(ps);

    } else if c == '\"' {
        state.pos += 1;
        let mut ps = ParseState {
            kind: ObjectType::String, buffer: "".to_string()
        };
        state.stack.push(ps);

    } else if c == '(' {
        state.pos += 1;
        let mut ps = ParseState {
            kind: ObjectType::List, buffer: "".to_string()
        };
        let mut nil = Arc::new(Mutex::new(Cons::Atom(Arc::new(Mutex::new(Atom::Nil)))));
        let new_ast = match &*state.ast.lock().unwrap() {
            Cons::Null => {
                state.current_cons = nil.clone();
                nil.clone()
            },
            Cons::Atom(atom) => {
                if let Atom::Nil = &*atom.lock().unwrap() {
                    let cons = Arc::new(Mutex::new(Cons::Cons(
                        Arc::new(Mutex::new(Cons::Null)),
                        Arc::new(Mutex::new(Cons::Atom(atom.clone())))
                    )));
                    state.current_cons = cons.clone();
                    cons.clone()
                } else {
                    panic!("wrong... ;  ~~unexpected list (or read twice?) {:?}~~", atom)
                }
            },
            Cons::Cons(_, _) => {
                let mut new_ast: Arc<Mutex<Cons>>;
                let new_curcons = if let Cons::Cons(car, cdr) = &*state.current_cons.lock().unwrap() {
                    new_ast = Arc::new(Mutex::new(Cons::Cons(car.clone(), nil.clone())));
                    nil.clone()
                } else {
                    new_ast = state.ast.clone();
                    state.current_cons.clone()
                };
                state.current_cons = new_curcons;
                new_ast
            },
        };
        state.ast = new_ast;
        state.stack.push(ps);

    } else {
        // read symbol
    }
 }

fn cheap_read_terminate(state: &mut Reader) {
    let ps = state.stack.pop().unwrap();
    match ps.kind {
        ObjectType::Integer => {
            if let Ok(i) = ps.buffer.parse::<i64>() {
                let int = Arc::new(Mutex::new(Cons::Atom(Arc::new(Mutex::new(Atom::Int(i))))));
                if let Some(top) = stack_top(state) {
                    println!("obj: {:?}", int);
                    if let ObjectType::List = state.stack[top].kind {
                        let new_curcons = match &*state.current_cons.lock().unwrap() {
                            Cons::Cons(car, cdr) => {
                                Arc::new(Mutex::new(Cons::Cons(int.clone(), state.current_cons.clone())))
                            },
                            Cons::Atom(atom) => {
                                Arc::new(Mutex::new(Cons::Cons(
                                    Arc::new(Mutex::new(Cons::Atom(atom.clone()))), state.current_cons.clone())
                                ))
                            },
                            _ => panic!("woops sexp is broken while parsing"),
                        };
                        state.current_cons = new_curcons;
                    } else {
                        state.ast = int;
                    }
                } else {
                    state.ast = int;
                }
            } else {
                panic!("'{}' is not a number", ps.buffer);
            }
        },
        ObjectType::Char => {
            state.ast = Arc::new(Mutex::new(Cons::Atom(
                Arc::new(Mutex::new(Atom::Char(ps.buffer.chars().nth(2).unwrap())))
            )));
        },
        ObjectType::String => {
            state.ast = Arc::new(Mutex::new(Cons::Atom(Arc::new(Mutex::new(Atom::Str(ps.buffer))))));
            state.pos += 1;
        },
        ObjectType::List => {
            state.pos += 1;
        },
        _ => (),
    }
}

fn cheap_read_non_terminate(state: &mut Reader) {
    if let Some(c) = state.input.chars().nth(state.pos) {
        match stack_top(state) {
            Some(pos) => match state.stack[pos].kind {
                ObjectType::Initial => {
                    cheap_read_dispatch(state);
                },
                ObjectType::Integer => {
                    if is_delimiter(c) {
                        cheap_read_terminate(state);
                    } else if c.is_digit(10) {
                        state.stack[pos].buffer.push(c);
                        state.pos += 1;
                    } else {
                        panic!("{} is not allowed in an integer", c);
                    }
                },
                ObjectType::Char => {
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
                },
                ObjectType::String => {
                    if c == '\"' {
                        cheap_read_terminate(state);
                    } else {
                        state.stack[pos].buffer.push(c);
                        state.pos += 1;
                    }
                },
                ObjectType::List => {
                    if c == ')' {
                        cheap_read_terminate(state);
                    } else if is_whitespace(c) {
                        state.pos += 1;
                    } else {
                        cheap_read_dispatch(state);
                    }
                },
                _ => (),
            },
            None => panic!("...unmatched what??"),
        }
    } else {
        panic!("unexpected EOF");
    }
}

fn cheap_read_1(state: &mut Reader) {
    cheap_read_non_terminate(state);
    loop {
        if state.pos >= state.input.len() {
            cheap_read_terminate(state);
            if let Some(top) = stack_top(state) {
                if top == 0 {
                    if let ObjectType::Initial = state.stack[top].kind {
                        state.stack.pop();
                        state.stack.push(ParseState { kind: ObjectType::EOF, buffer: "".to_string() });
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
                    if let ObjectType::Initial = state.stack[top].kind {
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

fn cheap_read(s: String) -> Vec<Arc<Mutex<Cons>>> {
    let mut cons_vec = Vec::new();
    let mut ast = Arc::new(Mutex::new(Cons::Null));
    let mut state = Reader {
        initial: true, input: s, pos: 0,
        ast: ast.clone(),
        current_cons: ast.clone(),
        stack: vec![ParseState { kind: ObjectType::Initial, buffer: "".to_string()}],
    };
    loop {
        cheap_read_1(&mut state);
        cons_vec.push(state.ast);
        // println!("stack: {:?}", state.stack);
        // println!("cons_vec: {:?}", cons_vec);

        state.initial = true;
        state.ast = Arc::new(Mutex::new(Cons::Null));
        state.current_cons = state.ast.clone();

        if let Some(top) = stack_top(&state) {
            if let ObjectType::EOF = state.stack[top].kind {
                break;
            }
        }
    };
    cons_vec
}


fn main() {
    let code = "12345 #\\1 () (1) \"hoge fuga\" (1 (2 (3 4) 5)) 123".to_string();
    println!("code: {:?}", code);
    let cons_vec = cheap_read(code);
    for cons in cons_vec {
        cheap_print(cons);
        println!("");
    }
}
