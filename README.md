# Geet – A Git Clone in C++ (Learning Project)

## About

This project follows the [Write Yourself a Git (WYAG)](https://wyag.thb.lt/) guide, which teaches git internals by reimplementing git from scratch. The original guide is in Python — I am translating it to **C++** as an additional learning challenge.

The project is called **geet** (a play on "git" + "g++" / "get it").

## Guide Reference

I am following the WYAG table of contents chapter by chapter:

1. Introduction
2. Getting started
3. Creating repositories: `init`
4. Reading and writing objects: `hash-object` and `cat-file`
5. Reading commit history: `log`
6. Reading commit data: `checkout`
7. Refs, tags and branches
8. Working with the staging area and the index file
9. Staging area and index, part 2: staging and committing
10. Final words


## Building (once I start coding)

```bash
# I'll set up my build system as part of the learning process
mkdir build && cd build
cmake ..
make
```

## Usage

```bash
./geet init [path]
./geet cat-file <type> <object>
./geet hash-object [-t <type>] [-w] <file>
./geet log <commit>
# ... and more
```
