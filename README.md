## Installation

To compile, you must have HElib already installed on your system.

1. Create build directory:

```bash
cd HElib-timing
mkdir build
cd build
```

2. Run the cmake configuration step:

```bash
cmake -Dhelib_DIR=<directory-to-installed-HElib>/share/cmake/helib ..
```

3. Compile, with an optional number of threads specified (16 in this example).

```bash
make -j16
```

The executable can be found in the `bin` directory.
