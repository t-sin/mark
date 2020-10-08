# Sciurus: language design

Here is some design memo for a Lisp named Sciurus.

## goal

Sciurus is a Lisp implementation.
Its goal is, for now, to implement a program called "Mark",
a text-basee interactive squirrel simulation program.

## features (goals)

Sciurus has these features below:

- Common Lisp based
    - reader system
    - macro system
    - stream based I/O
    - image oriented
    - multiple values
- some CL features are refined
    - case-sensitive reader
    - nested package system
    - standard features consists of several packages
- towards Mark
    - concurrency
    - event driven programming
    - text processing like regex
- compilation (on a virtual machine)

## language implementation policy

Sciurus is hosted on a virtual machine, for several reasons.
First, I thought it is handy to implement image oriented language.
Second, to gives scope to optimize some branching/jumping operations like `tagbody`/`go`.
On a tree-walking interpreter, I have no idea to optimize that situations.
Third, I have never implemented languages on virtual machines.

I want Sciurus to be "image oriented" like Common Lisp, Sciurus should have compilation API,
but I also want to provide abctraction for compilation details from Sciurus users.
Sciurus users should not care what Sciurus is running on, VM, interpreter, or as native code.
So compilation and image package (these are in *package design* section) are available after switching implementation laygers, for instance, a case of introducing native compiler.

## virtual machine

The virtual machine for Sciurus is Peter Landin's SECD machine based stack machine.

Supported types on VM are follows:

- bytes
- integers
- arrays
- conses

For concurrency, the VM will have multiple D registers and manupilate those to schedule multiple tasks
([@athos told me this idea. Thanks!](https://twitter.com/athos0220/status/1314071624287617024)).
To implement task scheduling I bought a book ["The Art of Concurrency"](https://github.com/Geal/nom/wiki/Error-management) (off cource, of Japanese translation)...

## TODO: filling the gap between the VM and Sciurus

## package design

- type
    - characters
    - arrays
    - strings
    - functions
    - conses
    - sequences (extensible)
    - structures
    - timestamps
    - symbols
    - packages
    - utilities
        - equality (eq, ...)
        - type-of, typep

- data structures
    - hash tables
    - (more data structures?)

- algorithm
    - sort
    - (more algorithms?)

- kernel
    - evaluation
    - macros
    - controll flows
        - primitives
            - block/return
            - go/tagbody
            - throw/catch
            - unwind-protect
        - if

- I/O
    - streams (extensible like gray streams)
    - reader
        - hash table literals
        - array literals
    - printer

- system
    - garabge collection
    - compilation/image dumping

- optional
    - CLOS-like object system
