Sudoku_Project
==============

Author Mei Gao,

gaomei.mun@gmail.com

Solving Sudoku Puzzle
The Sudoku problem is defined on a 9 by 9 grids table, and each grid is called a slot in our method. One number can only appear once in each row, column and 3 by 3 neighbour region.
This project uses two methods to solve a 9*9 Sudoku problem.
1) Branch bound method. Branch bound method is used to reduce search space of next slot for tradition backtracking algorithm. I also used number of possible values at a slot as a heristic function to decide next search slot.

2) A star Method. The objective value is number of open slot at each row, column, and 3 by 3 region, and hueristic function is number of possible value of each slot. Each round, A star algorithm will select a slot with minimal objective value + hueristic value, which also gaurantee optimal solution.

Both Branch bound and A star method is implemented in a char parsing framework.

In char parsing framework, two data structure is maintained,

1) Agenda is to determine what to do next. If it's a Queue, then it equals to BFS search, if a stack, DFS, and if priority queue, then a desired best first search

2) chart is the data structure used to process current data, prevent re-computing.

For more details, please refer to,

http://en.wikipedia.org/wiki/Chart_parser

The input file is defined at filePath in the third line of the main function. The file format follows the instruction of problem requirement.

Class SudokuSolver is the main class to solve Sudoku problem, and class Slot is a member class of class SudokuSolver.

To use the code,

1) Call readFromFile function to read Sudoku table as format of requirement.

2) Call solve function to solve Sudoku problem.

3) (Optional) Call print function to show result.
