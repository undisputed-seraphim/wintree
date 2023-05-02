# WinTree

A Windows port of the [`tree`](https://linux.die.net/man/1/tree) executable that list contents of directories in a tree-like format.

Work-in-progress.

### Why is this executable so big and fat? Unix `tree` was small!

Well I don't feel like rolling my own thousand-line flag selecting code so I'm just going to reuse Boost program_options.

## Building

You simply need CMake and a relatively recent version of Boost.program_options.

## To Do

- Figure out how to print unicode properly.
- Add support for all the other flags.
- Reduce memory allocation even more.

## License

MIT