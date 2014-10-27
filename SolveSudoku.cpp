//
//  main.cpp
//  Sudoku
//
//  Created by Mei Gao on 10/23/14.
//  Copyright (c) 2014 Mei Gao. All rights reserved.
//

// The input file is stored in a txt file, the suduku table is stored as a 81(9*9) length string in column major.
// each grid is defined as a slot, '0' represents the slot is empty, otherwise it should be from '1' to '9'


#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <string>
#include <memory>
#include <unordered_map>
#include <time.h>
#include <sstream>

// class Slot to store solution of one slot at (x_, y_)
class Slot  {
public:
    // Constructor
    Slot(int _x, int _y, std::vector<std::vector<int>> & _sudokuTable);
    
    // recover hypothesis from stack, and, manage stack
    void recover();
    
    // store the current state of hypothesis and g value to stack
    void store();
    
    // drop the top state from the stack
    void drop();
    
    // get value of current hypothesis
    int getVal();
    
    // get the score of the slot, if use branch bound method, return g_, if use A* method, return g_+h_
    int getScore();
    
    // search at next hypothesis
    bool nextSearch();
    
    // check whether the current hypotesis satisify sudoku constraint
    bool check();
    
    // compute the value of objective value g_
    void computeG();
    
    // compute the value of hueristc value of h_ in A* option
    void computeH();
    
    // compute the possible numbers of current location (x_, y_) in a given sudokuTable
    void computeHypothesis();
    
    // update hypothesis, g_ value and h_ value, if another slot is assigned a value
    void update(std::shared_ptr<Slot> slot);
    
    /// access function
    // get current location of x
    int& getX() {return x_;}
    
    // get current location of y
    int& getY() {return y_;}
    
    // get g value of the slot
    int& getG() {return g_;}
    
    // get h value of the slot
    int& getH() {return h_;}
    
private:
    
    // position of the slot, x is column and y is row
    int x_;
    int y_;
    
    int g_; // objective function value
    int h_; // heuristic function value

    // stack to store hypothesis, used in backtracking searchs
    std::stack<std::vector<int>> s_;
    
    // stack to store g value.
    std::stack<int>  sg_;
    
    std::vector<int> hypothesis_;
    
    // reference of sudoku table from class sudokuSolver
    std::vector<std::vector<int>> & sudokuTable_;
    
    /// index to hypothesis
    int idx_;
};

// comparison function to sort agenda
bool cmpFunc(std::shared_ptr<Slot> a, std::shared_ptr<Slot> b)
{
    return a->getScore() < b->getScore();
}

Slot::Slot(int _x, int _y, std::vector<std::vector<int>> & _sudokuTable): sudokuTable_(_sudokuTable) {
    x_ = _x;
    y_ = _y;
    idx_ = -1;
    g_ = 0;
    h_ = 0;
}


void Slot::recover()    {
    idx_ = -1;
    hypothesis_ = s_.top();
    g_ = sg_.top();
}

void Slot::store()  {
    s_.push(hypothesis_);
    sg_.push(g_);
}

void Slot::drop()   {
    s_.pop();
    sg_.pop();
}

int Slot::getVal()  {
    if(idx_ < hypothesis_.size())
        return hypothesis_[idx_];
    else
        return -1;
}

int Slot::getScore()    {
    return h_+g_;
}

bool Slot::nextSearch() {
    if( ++idx_ >= hypothesis_.size())   {
        sudokuTable_[y_][x_] = 0;
        return false;
    }
    sudokuTable_[y_][x_] = hypothesis_[idx_];
    return true;
}

bool Slot::check()  {
    // check if conflict at row y_
    for(int x = 0; x < 9; x++)  {
        if(x != x_ && sudokuTable_[y_][x] == getVal())
            return false;
    }
    
    // check if conflict at column x_
    for(int y = 0; y < 9; y++)  {
        if(y != y_ && sudokuTable_[y][x_] == getVal())
            return false;
    }
    
    // check if conflict in a 3*3 region
    for(int y = y_/3 * 3; y < y_/3*3+3; y++)  {
        for(int x = x_/3 * 3; x < x_/3*3+3; x++)    {
            if( !(y == y_ && x == x_) && sudokuTable_[y][x] == getVal())
                return false;
        }
    }
    return true;
}

void Slot::computeG()    {
    // g = 20 - non-empty slot
    g_ = 0;
    // count how many cols not zero
    for(int x = 0; x < 9; x++)  {
        if(x != x_ && sudokuTable_[y_][x] != 0)
            g_++;
    }
    
    // count how many rows not zero
    for(int y = 0; y < 9; y++)  {
        if(y != y_ && sudokuTable_[y][x_] != 0)
            g_++;
    }
    
    // count how many 3*3 not zero
    for(int y = y_/3 * 3; y < y_/3*3+3; y++)  {
        for(int x = x_/3 * 3; x < x_/3*3+3; x++)    {
            if( !(y == y_ && x == x_) && sudokuTable_[y][x] != 0)
                g_++;
        }
    }
    g_ = 20 - g_;
}

void Slot::computeH()   {
    // h = # of hypothesis
    h_ = int(hypothesis_.size());
}

void Slot::computeHypothesis() {
    std::unordered_map<int, bool> dicMap;
    // find exist value at row y_
    for(int x = 0; x < 9; x++)  {
        if(x != x_ && sudokuTable_[y_][x] != 0)
            dicMap[sudokuTable_[y_][x]] = true;
    }

    // find exist value at column x_
    for(int y = 0; y < 9; y++)  {
        if(y != y_ && sudokuTable_[y][x_] != 0)
            dicMap[sudokuTable_[y][x_]] = true;
    }
    
    // find exist value at 3*3 neighbor region
    for(int y = y_/3 * 3; y < y_/3*3+3; y++)  {
        for(int x = x_/3 * 3; x < x_/3*3+3; x++)    {
            if( !(y == y_ && x == x_) && sudokuTable_[y][x] != 0)
                dicMap[sudokuTable_[y][x]] = true;
        }
    }
    hypothesis_.resize(9 - dicMap.size());
    int idx = 0;
    
    // find the possible hypothesis
    for(int i = 1; i <=9; i++)  {
        if( dicMap.find(i) == dicMap.end())
            hypothesis_[idx++] = i;
    }
    
    idx_ = -1;
}

void Slot::update(std::shared_ptr<Slot> slot)   {
    
    if(x_ == slot->getX() || y_ == slot->getY() || ( x_/3 == slot->getX()/3 && y_/3 == slot->getY()/3)) {
        // update g
        g_--;
        
        for(int i = 0; i < hypothesis_.size(); i++) {
            if(hypothesis_[i] == slot->getVal())    {
                // if hypotesis contains input slot's value, remove it
                hypothesis_[i] = hypothesis_.back();
                hypothesis_.resize(hypothesis_.size() - 1);
                break;
            }
        }
    }
    idx_ = -1;
}

// class to solve sudoku puzzle problem, I use the chart parsing to implement A star search and branch bound search
//Basic idea of chart parsing: Don't throw away any information. Keep a record --- a chart --- of all the structures we have found.
/*
 A chart is a form of well-formed substring table [Partial parse graph],
 – It plays the role of the memo-table as in DP.
 – It keeps track of partial derivations so nothing has to be rederived
 
 An agenda is a data structure that keeps track of the things we still have to do. Simply put, it is a set of edges waiting to be added to the chart
 
 The agenda determines in what order edges are added to the chart
 – Stack agenda for depth-first search
 – Queue agenda for breadth-first search
 – Priority queue agenda for best-first search
 
 */
// each step add the optimal search node from agenda to chart. In this implenmation, agenda is a priority queue, which can be sorted based on their score = g_ + h_, and chart is a stack used for backtracking.

class SudokuSolver    {
public:
    // search method option
    enum searchMethod{branchBound = 0, Astar};
    
    // constructor
    SudokuSolver();
    
    // constructor with search method
    SudokuSolver(searchMethod _searchMethod);
    
    // read sudoku puzzle from a file
    bool readFromFile(std::string fileName);
    
    // print sudoku table
    void print();
    
    // solve sudoku puzzle, if have solution return true, otherwise return false
    bool solve();
    
private:
    
    ///function
    
    // init function to add empty
    void init();
    
    /// compute g and h
    void processAgenda();
    
    /// use shared_ptr to strore agenda so that keep efficiency and prevent memory leakage
    std::deque<std::shared_ptr<Slot>> agenda_;
    std::stack<std::shared_ptr<Slot>> chart_;
    std::vector<std::shared_ptr<Slot>> solution_;
    
    std::vector<std::vector<int>> sudokuTable_;
    
    searchMethod searchMethod_;
};

SudokuSolver::SudokuSolver()    {
    sudokuTable_.resize(9);
    for(int i = 0; i < 9; i++)
        sudokuTable_[i].resize(9);
    
    searchMethod_ = searchMethod::branchBound;
}

SudokuSolver::SudokuSolver(searchMethod _searchMethod) {
    sudokuTable_.resize(9);
    for(int i = 0; i < 9; i++)
        sudokuTable_[i].resize(9);
    
    searchMethod_ = _searchMethod;
}

bool SudokuSolver::solve() {
    // preprocessing agenda, compute hypothesis, g value and h value of each slot
    for (auto e: agenda_) {
        e->computeHypothesis();
    }
    processAgenda();
    
    // store current state to stacks
    for(auto e : agenda_)
        e->store();
    
    // add the best guess slot to chart to process
    chart_.push(agenda_.front());
    agenda_.pop_front();
    
    
    while(! chart_.empty()) {
        // process top element in chart
        std::shared_ptr<Slot> ptr = chart_.top();
        
        // search next hypothesis of the slot
        if(! ptr->nextSearch()) {
            // pop chart to agenda
            //put element back to agenda
            agenda_.push_front(chart_.top());
            chart_.pop();
            
            //recover state of slots in agenda
            for(auto e:agenda_) {
                e->recover();
                e->drop();
            }
            continue;
        }
        
        
        // check if current hypothesis of the slot satisify sudoku constraint.
        if (ptr->check()) {
            
            // if satisify, store current state of slot in agenda
            for(auto e : agenda_)   {
                e->store();
                
                // update g value, h value, hypothsis
                e->update(ptr);
                
                // if A star, compute h value
                if(searchMethod_ == searchMethod::Astar)
                    e->computeH();
            }
            
            // if agenda is empty, store solution, assuming only one solution
            if(agenda_.empty()) {
                while( !chart_.empty()) {
                    solution_.push_back(chart_.top());
                    chart_.pop();
                }
                return true;
            }
            
            // sort agenda, so that each time select the slot with minimal score. If use branch bound function, it is equalvelent to use g as hueristic information, and if use A star, it is exactly A star algorithm
            std::sort(agenda_.begin(), agenda_.end(), cmpFunc);
            
            // move optimal slot from agenda to chart
            chart_.push(agenda_.front());
            agenda_.pop_front();
            
            continue;
        }
        
    }
    
    return false;
}


bool SudokuSolver::readFromFile(std::string fileName) {
    std::ifstream myFile(fileName);
    if(! myFile.is_open())
        return false;
    std::string input;
    
    for(int i = 0; i < 9; i++)  {
        if(!getline(myFile, input))
            return false;
        
        std::istringstream ss( input );
        for(int j = 0; j < 9; j++)  {
            std::string s;
            if (!getline( ss, s, ',' ))
                return false;
            
            if(s.length()!= 1 || s[0] <'0' || s[0] > '9')
                return false;
            
            if(s[0] == 0)
                sudokuTable_[i][j] = 0;
            else
                sudokuTable_[i][j] = s[0] - '0';
            
        }
        
    }
    
    // initialize slot structure and agenda
    init();
    return true;
}

void SudokuSolver::init()   {
    agenda_.clear();
    while(!chart_.empty())
        chart_.pop();
    
    solution_.clear();
    
    for(int y = 0; y < 9; y++)  {
        for(int x = 0; x < 9; x++)  {
            if( sudokuTable_[y][x] == 0)    {
                // allocate slot class to store empty grid
                std::shared_ptr<Slot> slot = std::make_shared<Slot>(x, y, sudokuTable_);
                agenda_.push_back(slot);
            }
        }
    }
}

void SudokuSolver::print()  {
    std::cout << std::endl;
    for(int r = 0; r < 9; r++)  {
        for(int c = 0; c < 9; c++)  {

            std::cout << sudokuTable_[r][c] << ' ';
            if(c == 2 || c == 5)
                std::cout << '|';
        }
        std::cout << std::endl;
        if(r == 2 || r == 5)
            std::cout << "------+------+------" <<std::endl;
    }
    
    std::cout << "********************" << std::endl;
}

void SudokuSolver::processAgenda()    {
    
    for(auto e : agenda_)   {
        e->computeG();
        if(searchMethod_ == searchMethod::Astar)
            e->computeH();
    }
    
    std::sort(agenda_.begin(), agenda_.end(), cmpFunc);
    
}

int main(int argc, const char * argv[]) {
    // insert code here...
    SudokuSolver sudokuSolver(SudokuSolver::Astar);
    
    std::string filePath = "/Users/meigao/Documents/Projects/Sudoku/3.txt";
    
    /* simple use
     sudokuSolver.readFromFile(filePath);
     // print before solve
     sudokuSolver.print();
     sudokuSolver.solve();
     // print after solved
     sudokuSolver.print();
     */
    
    if(!sudokuSolver.readFromFile(filePath))    {
        std::cout << "File read error!!" << std::endl;
        return 0;
    }
    sudokuSolver.print();
    
    clock_t t;
    if(sudokuSolver.solve())    {
        t = clock() - t;
        sudokuSolver.print();
        std::cout << "Total time used is " << ((float)t)/CLOCKS_PER_SEC << "seconds" << std::endl;
    }
    else    {
        t = clock() - t;
        std::cout << "Total time used is " << ((float)t)/CLOCKS_PER_SEC << "seconds" << std::endl;
        std::cout << "No Solution." << std::endl;
    }
    
    return 1;
}
