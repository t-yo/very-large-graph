#  Fast Very Large Graph Diameter Estimations Using Quotient Graph


## Context 

Research project in C which has for goal to find an efficient way to estimate Very Large Graph's diameters. To do so we tried different techniques using the quotient graph.

This project was developped by Yossra ANTARI, Axen GEORGET and Alex VAN VLIET during our 4th year at EPITA and supervised by our professor, Robert Erra.

To have more context and see our results you can read our report named 'vlg-report' in this repository.

## Build

The project can be built with cmake.
First, create the directory where you want to build the project.
This directory will be called `<BUILD_DIR>` afterwards.
Then, run the following commands:

```sh
cd <BUILD_DIR>
cmake <SOURCE_DIR> -DCMAKE_BUILD_TYPE=<BUILD_TYPE>
cmake --build .
```

The `<BUILD_TYPE>` can be either:
- `Debug`:
This enables the debug flags, asan and warnings as errors.
- `Release`:
This enables all optimizations.


## Author

Yossra Antari: yossra.antari@epita.fr
Axen Georget: axen.georget@epita.fr
Alex Van Vliet: alex.van-vliet@epita.fr
