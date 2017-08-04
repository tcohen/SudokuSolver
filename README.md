# SudokuSolver

I wrote this on an airplane after being bored and then frustrated that I was having trouble solving the Sudoku problems from the magazine kindly provided in the seat pocket in front of me.

> I decided that backtracking is totally inappropriate for the problem.  Each square must be solved by exclusion rather than by inclusion.  In other words, it's not correct to fill a square with any value that currently satisfies the rules, because you're likely to eventually get stuck.  Rather, you must only fill a square when there is exactly one remaining possible value for that square.
It actually works out elegantly.  If you ever find a board where *every* square can take more than one value, then the board is under-specified (not enough information).  And if you ever find a board where a square cannot take any value, then the board is over-specified (no solution exists).

> Yes, no backtracking, only direct deduction.  And my solution is pretty efficient with no brute-force or trial-and-error elements.
It goes through the givens, and propagates exclusions through the rows, columns, and blocks.  As it goes along, if a square is found to have 8 excluded values, then it pushes that square as a newly discovered/solved square.  When it's done propagating, it looks at that queue and propagates the newly solved squares, and so forth until the number of solved squares equals the number of board squares.

> And yes, it was much more fun for me to write the solver than to solve any problems by hand. :-)
