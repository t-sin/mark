# Mark - A squirrel who implemented

> Mark wakes up.
> --- "The Squirrel Awakes" by Toh Enjoe

## Goals

* Understanting Mark
* Understanting Toh Enjoe

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
ninja -C build/ lis
```

## TODOs

- data types & operations
  - strings
  - timestamps
  - first-class functions
- CL-like reader
- multi-threading? (to interact with Mark)
- (macro system?)
- lifecycle of squirrels

## License

This program *Mark* is licensed under the GNU General Public License Version 3. See [COPYING](COPYING) for details.
