# Geet – A Git Clone in C++

## About

**geet** is a custom implementation of Git from scratch, written in **C++**. It reimplements Git internals and core functionalities, serving as a functional version control tool that operates on standard Git repositories.

## Building

```bash
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
