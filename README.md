# Mark - A squirrel who implemented

> Mark wakes up.
> --- "The Squirrel Awakes" by Toh Enjoe

## Goals

* Understanting Mark
* Understanting Toh Enjoe

## TODOs

- [x] fundamentals
  - [x] byte streams
  - [x] UTF-8 encode/decode
  - [x] object layout
  - [x] hash tables
- [x] primitive types
  - [x] characters and strings
  - [x] integers
  - [x] symbols (without packages)
  - [x] streams
  - [x] conses and lists
  - [x] environments
  - [x] function objects
  - [x] lexical closures
  - [x] packages
  - [ ] timestamps
- [ ] operations
  - [x] arithmatic operations for integers
  - [ ] string operations
  - [x] cons operations
- [ ] REPL
  - [x] cheap reader
  - [x] eval
  - [x] print
  - [ ] reader with readtable
- [ ] advanced features
  - [ ] garbage collection
  - [ ] multi-threading? (to interact with Mark)
  - [ ] macro system?
- [ ] building Mark
  - [ ] lifecycle of Mark
  - [ ] DSL for Mark
  - [ ] implement Mark

## Requirements

Nothing yet

## Build

To build, first, install [meson](https://mesonbuild.com/) and [ninja](https://ninja-build.org/). And type this:

```sh
$ cd /path/to/mark
$ meson build/
$ ninja -C build/
```

So executable `lis` will be created at `build/lis`. To run `lis`, type

```sh
$ ./build/src/lis
```

or

```sh
ninja -C build/ start
```

Some basic tests can be run with those command:

```sh
ninja -C ./build test
```

## License

This program *Mark* is licensed under the GNU General Public License Version 3. See [COPYING](COPYING) for details.
