# SudokuSolver-DancingLinks
Implementation of the extremely fast algorithm X, using dancing links as main datastructure , solving sudokus.
Solves the top 95 hardest sudokus in  approximately 0.05 seconds.

https://arxiv.org/pdf/cs/0011047v1.pdf

![Links](https://raw.githubusercontent.com/BNandor/SudokuSolver-DancingLinks/master/img/links.jpg)

## Usage
>./DLX input-filename

## Input format

A puzzle must be in a single row, empty cells are represented as dots:
For example:
>3...8.......7....51..............36...2..4....7...........6.13..452...........8..

is a valid input.

The executable solves all the puzzles in the input file.

### Other options
##### -Dmatrixform //print solutions in matrix form
##### -Dtotal //print total number of solutions
##### -Dmore //check all solutions,do not stop at first one
