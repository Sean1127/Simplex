#ifndef SIMPLEX_H
#define SIMPLEX_H
#define M 1000
#define ROW 100
#define COL 100

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <cassert>

using namespace std;

static const double epsilon = 1.0e-8;

class Simplex
{
public:
    Simplex();
    void print();
    void read(string);
    void pivot_on(int, int);
    int find_pivot_column();
    int find_pivot_row(int);
    int find_basis_variable(int);
    void print_optimal_vector();
    void solve();
private:
    string type;
    int m;
    int n;
    double mat[ROW][COL];
};

#endif // SIMPLEX_H

