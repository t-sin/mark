# Mark A

*Mark A* is a program that describe about a squirrel Mark as strings. *Mark A*'s behavior is described with the language Sciurus.

## `mark` command

`mark` command is an executer program to behave as written in Sciurus file (`*.sciurus`). This has features below:

- customizable behavior with some events
  - per-tick events
  - time events
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
  -t                   Print timestamps at the head of outputs.
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
- random number generation
- event registration operations to the run-time scheduler
  - per-tick events
  - time events
  - composing events above
- run-time variables

## Behavior examples


```
;; print "hello" at every ticks
(define-tick-event 'tick-hello
  (println "hello"))
```

```
;; print current time
(define-tick-event 'now
  (println "hi, it's %t" (now)))

;; raise an argument error at LOAD-time
(define-tick-event 'now
  (println "hi, it's %t"))
```

```
;; print every 9am UTC
(define-time-event 'morning T"09:00:00"
  (println "morning, it's 9 o'clock!"))
```

```
;; conditional printing with run-time variable USER
(define-time-event 'morning-for-mark T"09:00:00"
  (when (= runtime:user "Mark")
    (println "morning, Mark!")))
```

```
;; formatted output
(define-time-event 'morning-for-user T"09:00:00"
  (if (null runtime:user)
    (println "morning, man!")
    (println "morning, %s!" runtime:user)))
```

```
;; internal states
(defvar count 0)

(define-tick-event 'notify-every-10-ticks
  (when (= (% count 10) 0)
    (println "count %n" count))
  (set count (+ count 1)))
```

```
;; probabilistic outputs (50%)
(define-tick-event 'notify-at-random
  (when (= (random 1) 0)
    (println "hi!")))
```

## memo

- to schedule events, use POSIX [`settimer()`](https://linuxjm.osdn.jp/html/LDP_man-pages/man2/getitimer.2.html) and handling UNIX signals
- for time information, use [`gmtime_r()`](https://linuxjm.osdn.jp/html/LDP_man-pages/man3/ctime.3.html)
