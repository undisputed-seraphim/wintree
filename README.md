# WinTree

A Windows port of the [`tree`](https://linux.die.net/man/1/tree) executable that list contents of directories in a tree-like format.

Work-in-progress.

## Building

You simply need CMake. No dependencies are required. Note that this project uses win32 APIs and therefore does not compile on operating systems that are not Windows.

## To Do

- Figure out how to print unicode properly.
- Add support for all the other flags.
- Reduce memory allocation even more.

## License

MIT