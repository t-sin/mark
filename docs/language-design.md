# Sciurus: language design

## basic features

- Common Lisp based
    - reader system
    - macro system
    - stream based I/O
    - image oriented
- Some CL features are refined
    - case-sensitive characters
    - nested package system
    - standard features consists of several packages
- foreign function interface
- core features are separated into each packages
- compile feature (on a virtual machine)
- threading or asynchronous processing (for Mark)
- event driven programming (for Mark)
- text processing like regex (for Mark)

### package design

- type
    - characters
    - strings
    - functions
    - conses
    - sequences (extensible)
    - utilities
        - equality (eq, ...)
        - type-of, typep

- data structures
    - hash tables
    - arrays
    - (more data structures?)

- algorithm
    - sort

- kernel features
    - symbols
    - packages
    - evaluation
    - macros
    - controll flows
        - primitives
            - block/return
            - go/tagbody
            - throw/catch
            - unwind-protect
        - if

- I/O features
    - streams (extensible like gray streams)
    - reader
        - hash table literals
        - array literals
    - printer

- system features
    - garabge collection
    - compilation/image dumping

- additional features
    - CLOS-like object system
