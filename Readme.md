SVM
===

A simple virtual machine to illustrate core concepts of a computer system.

## Compilation

### Most Unix/Unix-like Systems (including the Cygwin environment and Mac OS)

    cd build/
    cmake ..
    make

### Windows with Visual Studio

1. Get the latest version of CMake from `https://cmake.org/download`.
2. Install or unpack the archive.
3. Start the GUI version of CMake.
4. Set the source directory to the root of the `svm/`
5. Set the build directory to `svm/build`
6. Press `Configure`
7. Ensure that there are no errors.
8. Press `Generate`
9. Open the generated Visual Studio solution file from the build directory.

### Mac OS with Xcode

Install CMake from sources, Homebrew, or MacPorts. Generate an Xcode project and
open it.

    cd build/
    cmake -G Xcode ..

## Usage

### SVM

`svm <scheduler name> <.vmexe file> <.vmexe file>...`

Check the `main` function in `svm.c` for a list of schedulers.

`.vmexe` is a compiled executable for a simple virtual CPU architecture used in
SVM. `.vmexe` files are translated from `.vmasm` sources by SVMASM. A number of
sample sources can be found in the `assemblies` directory. The build system will
automatically translate two samples into executables with SVMASM. You can find
them in the `build/assemblies/`. For your own `.vmasm` programs you have to run
SVMASM manually.

SVM loads executables into memory and creates processes for each image. It then
starts execution of the first process on the virtual CPU sequentally.

Your task is to write implementation for each scheduler in `kernel.c` and
`kernel.h`. Use approaches described in the course book.

### SVMASM

`svmasm <source .vmasm> <output .vmexe>`

SVMASM translates source assembly code into binary executables that can be used
with the virtual CPU in SVM.

Use it to generate executables for your own `.vmasm` programs.
