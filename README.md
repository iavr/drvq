[drvq](http://image.ntua.gr/iva/tools/drvq)
====

*Dimensionality-recursive vector quantization*

`drvq` is a C++ library implementation of [dimensionality-recursive vector quantization](http://image.ntua.gr/iva/research/drvq), a fast vector quantization method in Euclidean spaces under arbitrary data distributions.

Licence
----

`drvq` has 2-clause BSD license. See file LICENSE for the complete license text.

Directory structure
----

`drvq` library constists primarily of template C++ header-only (`.hpp`) code, but also includes a number of tools to test the library, provided as `.cpp` files. The directory structure is:

	/data      data files for testing
	/matlab    binary file input/output tools for matlab
	/out       output folder for drvq tools
	/run       simple scripts to compile and execute
	/src       drvq library and tools source code

### Requirements

`drvq` implementation is based on C++ template library [ivl](http://image.ntua.gr/ivl/), which may be downloaded from [sourceforge.net](http://sourceforge.net/projects/ivl/files/) and [installed](http://image.ntua.gr/ivl/download.php) on a number of platforms.

### Installation, building

No installation is needed. Command-line tools under folder `src` illustrate how the library may be used and may be compiled with minimal options without a Makefile; scripts under folder `run/` show how a tool may be compiled and executed, using either `Clang` or `GCC`.

`drvq` has only been tested on [clang 3.3](http://llvm.org/releases/download.html#3.3) and [g++ 4.8.1](http://gcc.gnu.org/gcc-4.8/) on Linux, but it should be straightfoward to use on other platforms.

Script `/run/run` is for g++ and `/run/lrun` for clang. It is best to copy them in a folder in your path, e.g. `/usr/local/bin`, after adjusting folders for your local copy of `ivl`. In this case,

	cd src/
	run train

compiles file `/src/train.cpp`, produces binary `/src/train`, and executes it unless there are compiler or linker errors.

Extension `.cpp` is not necessary for the main source file. Additional source files or compiler options may be specified as additional command-line arguments, but extensions are needed for extra source files.

### Binary file format

### drvq tools

### Citation

Please cite the following paper if you use this software.

Yannis Avrithis. [Quantize and Conquer: A dimensionality-recursive solution to clustering, vector quantization, and image retrieval](http://image.ntua.gr/iva/publications/qc). In Proceedings of International Conference on Computer Vision (ICCV 2013), December 2013.
