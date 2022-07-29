# Mark A

*Mark A* is a program that describe about a squirrel Mark as strings. *Mark A*'s behavior is described with the language Sciurus.

## `mark` command

`mark` command is an executer program to behave as written in Sciurus file (`*.sciurus`). This has features below:

- customizable behavior with some events
  - per-minutes events
  - time events
  - probabilistic events
  - complex events with internal states and composing events above
- switch behavior via run-time variables

```
mark COMMAND [OPTIONS] SCIURUS_FILE

COMMAND
    run                Run the behavior defined in the Sciurus source file.
    describe           Check the Sciurus source file and describe scheduler behavior defined in it.

    help               Print this help.
    version            Print this mark's version.

OPTIONS
  -v NAME=VALUE        Set run-time variable VALUE as NAME.
  --verbose            Print scheduler behaviors.
```

## The Sciurus language

The Sciurus language features:

- integers
- Unicode strings and UTF-8 IO
- user-defined functions
- lexical bindings
- compound types
  - structures
  - arrays
- simple string operations
  - concatenation
  - slicing
  - index accessing
- formatted string outputs
- event registration operations to the run-time scheduler
  - per-minutes events
  - time events
  - probabilistic events
  - composing events above
- run-time variables
