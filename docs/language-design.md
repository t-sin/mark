# Sciurus: Language design

## basic features

- Common Lisp based
    - reader system
    - macro system
    - stream based I/O
    - image oriented
- Some CL features are refined
    - case-sensitive characters
    - package splitted standard features
- foreign function interface
- core features are separated into each packages
- compile feature (on a virtual machine)
- threading or asynchronous processing (for Mark)
- event driven programming (for Mark)
- text processing like regex (for Mark)

### package design

- types
    - characters
    - strings
    - symbols
    - packages
    - functions
    - conses
    - sequences (extensible)
    - equality (eq, ...)

- data structures
    - hash tables
    - arrays
    
- kernel features
    - evaluation
    - macros
    - controll flows
        - block/return
        - go/tagbody
        - throw/catch
        - if
    - object systems?

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
    - object system
