#include "simplex.h"

int equal(double a, double b)
{
    return fabs(a-b) < epsilon;
}

void nl(int k)
{
    int i;
    for (i = 0; i < k; ++i)
        cout << "-";
    cout << endl;
}

Simplex::Simplex()
{
}

void Simplex::print()
{
    static int counter = 0;
    int i, j;
    cout << endl;
    nl(70);
    cout << counter++ << "." << endl;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            cout << setw(8) << right << mat[i][j] << " ";
        }
        cout << endl;
    }
}

/*
    file format:

    max z = 3x1 + 5x2
    1x1 + 0x2 <= 4
    0x1 + 2x2 <= 12
    3x1 + 2x2 <= 18

    note:
    use max/min to maximize/minimize objective z
    use space to seperate any operand and operator
    use only plus sign '+', no minus sign '-'
    accept <=, =, >= for the constraints
*/

void Simplex::read(string filename)
{
    ifstream infile(filename.c_str());
    if (!infile.is_open()) {
        cerr << "cannot open file" << endl;
        exit(1);
    }

    int i, j, n0;
    size_t pos;
    string line, token;

    // min or max
    getline(infile, line);
    pos = line.find(" ");
    type = line.substr(0, pos);
    line = line.substr(line.find("=") + 2);

    // objective coefficient
    for (j = 1; (pos = line.find(" ")) != string::npos; ) {
        token = line.substr(0, pos);
        if (token.compare("+")) {
            token = token.substr(0, token.find("x"));
            mat[0][j++] = -atof(token.c_str());
        }
        line = line.substr(pos + 1);
    }
    token = line.substr(0, line.find("x"));
    mat[0][j++] = -atof(token.c_str());
    n0 = j;
    n = j;

    // constraint coefficient
    for (i = 1; getline(infile, line); i++) {
        for (j = 1; (pos = line.find(" ")) != string::npos; ) {
            token = line.substr(0, pos);
            if (!token.compare("=")) {          // add slack, artificial
                mat[i][n] = 1;
                mat[0][n++] = M;
            } else if (!token.compare(">=")) {  // add slack, artificial, surplus
                mat[i][n++] = -1;
                mat[i][n] = 1;
                mat[0][n++] = M;
            } else if (!token.compare("<=")) {  // add slack
                mat[i][n++] = 1;
            } else if (token.compare("+")) {
                token = token.substr(0, token.find("x"));
                mat[i][j++] = atof(token.c_str());
            }
            line = line.substr(pos + 1);
        }
        mat[i][0] = atof(line.substr(0, line.find("x")).c_str());
    }
    m = i;

    // objective change
    if (!type.compare("min")) {
        for (j = 1; j < n0; j++) {
            mat[0][j] = -mat[0][j];
        }
    }

    // eliminate artificial
    int eq[ROW];
    int count = 0;
    for (j = 1; j < n; j++) { // find big M rows
        if (mat[0][j] == M || mat[0][j] == -M) {
            for (i = 1; i < m; i++) {
                if (mat[i][j]) {
                    eq[count++] = i;
                    break;
                }
            }
        }
    }
    for (i = 0; i < count; i++) { // row operation
        for (j = 0; j < n; j++) {
            mat[0][j] -= M * mat[eq[i]][j];
        }
    }

    cout << "file: " << filename << endl;
    cout << "row: " << m << endl;
    cout << "col: " << n << endl;
    infile.close();
}

void Simplex::pivot_on(int row, int col)
{
    int i, j;
    double pivot;
    
    pivot = mat[row][col];
    assert(pivot > 0);
    for (j = 0; j < n; j++)
        mat[row][j] /= pivot;
    assert(equal(mat[row][col], 1));
    
    for (i = 0; i < m; i++) { // elementary row operation
        double multiplier = mat[i][col];
        if (i == row)
            continue;
        for (j = 0; j < n; j++) {
            mat[i][j] -= multiplier * mat[row][j];
        }
    }
}

// most negative column in row[0]
int Simplex::find_pivot_column()
{
    int j, pivot_col = 1;
    double lowest = mat[0][pivot_col];

    for (j = 1; j < n; j++) {
        if (mat[0][j] < lowest) {
            lowest = mat[0][j];
            pivot_col = j;
        }
    }
    cout << "pivot col: " << pivot_col << " = " << lowest << endl;

    if (lowest >= 0) { // optimality test
        return -1; // every coefficient in row[0] is nonnegative
    }
    return pivot_col;
}

// minimum ratio test: col[0]/col[row]
int Simplex::find_pivot_row(int pivot_col)
{
    int i, pivot_row = 0;
    double min_ratio = -1;
    cout << "min{";
    for (i = 1; i < m; i++) {
        double ratio = mat[i][0] / mat[i][pivot_col];
        cout << ratio << ", ";
        if ((ratio > 0 && ratio < min_ratio) || min_ratio < 0) {
            min_ratio = ratio;
            pivot_row = i;
        }
    }
    cout << "}" << endl;
    if (min_ratio == -1)
        return -1; // unbounded z
    cout << "pivot row: " << pivot_row << endl;
    return pivot_row;
}

// Given a column of identity matrix, find the row containing 1.
// return -1, if the column as not from an identity matrix.
int Simplex::find_basis_variable(int col)
{
    int i, xi = -1;
    for (i = 1; i < m; i++) {
        if (equal(mat[i][col], 1)) {
            if (xi == -1)
                xi = i;   // found first '1', save this row number.
            else
                return -1; // found second '1', not an identity matrix.
        } else if (!equal(mat[i][col], 0)) {
            return -1; // not an identity matrix column.
        }
    }
    return xi;
}

void Simplex::print_optimal_vector()
{
    int j, xi;
    for (j = 1; j < n; j++) { // for each column
        xi = find_basis_variable(j);
        if (xi != -1)
            cout << "x" << j << " = " << mat[xi][0] << ", ";
        else
            cout << "x" << j << " = 0, ";
    }
    cout << endl;
}

void Simplex::solve()
{
    int loop = 0;
    while (++loop) {
        int pivot_col, pivot_row;
        
        pivot_col = find_pivot_column();
        if (pivot_col < 0) {
            if (!type.compare("min"))
                mat[0][0] *= -1;
            cout << "optimal solution z = " << mat[0][0] << endl;
            print_optimal_vector();
            break;
        }
        
        pivot_row = find_pivot_row(pivot_col);
        if (pivot_row < 0) {
            cout << "unbounded z" << endl;
            break;
        }
        
        pivot_on(pivot_row, pivot_col);
        print();
        print_optimal_vector();
        
        if(loop > 20) {
            cout << "iterations > " << loop << endl;
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    Simplex simplex;
    if (argc > 1) { // usage: cmd datafile
        simplex.read(argv[1]);
    }
    simplex.print();
    simplex.solve();
    return 0;
} 
