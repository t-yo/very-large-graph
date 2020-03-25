# Very Large Graph - Dream Team

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
