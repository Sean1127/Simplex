#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cmath>

#define M 1000

using namespace std;

static const double epsilon = 1.0e-8;

class Tableau {
public:
    int m;
    int n;
    double **mat;
};

int equal(double a, double b) { return fabs(a-b) < epsilon; }

void nl(int k) {
    int i;
    for (i = 0; i < k; ++i)
        cout << "-";
    cout << endl;
}

void print_tab(Tableau tab, const string msg) {
    static int counter = 0;
    int i, j;
    cout << endl;
    cout << ++counter << ". Tableau " << msg << ":" << endl;
    for (i = 1; i < tab.n; i++)
        cout << "    x" << i << ",";
    cout << endl;
    for (i = 0; i < tab.m; i++) {
        if (i == 0)
            cout << "max:";
        else
            cout << "b" << i << ": ";
        for (j = 0; j < tab.m; j++) {
            if (equal((int) tab.mat[i][j], tab.mat[i][j]))
                cout << setw(7) << right << tab.mat[i][j];
            else
                cout << setw(7) << right << tab.mat[i][j];
        }
        cout << endl;
    }
    nl(70);
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

void read_tab(Tableau tab, const char* filename) {
    ifstream infile(filename);
    if (!infile.is_open()) {
        cerr << "cannot open file" << endl;
        exit(1);
    }

    int i, j;
    size_t pos;
    string line;
    string token;

    getline(infile, line);
    token = line.substr(0, line.find(" "));
    if (!token.compare("max")) {
        // stay unchanged
    } else if (!token.compare("min")) {
        //note min;
    } else {
        cerr << "format" << endl;
        exit(1);
    }

    for (j = 1; (pos = line.find("x")) != strring::npos; j++) {
        token = line.substr(0, pos);
        tab.mat[i][j] = atof(token.c_str());
        line = line.substr(pos + 1);
    }
    tab.n = j;

    for (i = 1; getline(infile, line); i++) {
        for (j = 1; () != ; j++) {
        }
    }

    infile.close();
    
    tab.m = i;
    cout << "Read tableau [" << tab.m << " rows x " << tab.n << " columns] from file '" << filename << "'." << endl;
}
/*
void pivot_on(Tableau *tab, int row, int col) {
  int i, j;
  double pivot;

  pivot = tab->mat[row][col];
  assert(pivot>0);
  for(j=0;j<tab->n;j++)
    tab->mat[row][j] /= pivot;
  assert( equal(tab->mat[row][col], 1. ));

  for(i=0; i<tab->m; i++) { // foreach remaining row i do
    double multiplier = tab->mat[i][col];
    if(i==row) continue;
    for(j=0; j<tab->n; j++) { // r[i] = r[i] - z * r[row];
      tab->mat[i][j] -= multiplier * tab->mat[row][j];
    }
  }
}

// Find pivot_col = most negative column in mat[0][1..n]
int find_pivot_column(Tableau *tab) {
  int j, pivot_col = 1;
  double lowest = tab->mat[0][pivot_col];
  for(j=1; j<tab->n; j++) {
    if (tab->mat[0][j] < lowest) {
      lowest = tab->mat[0][j];
      pivot_col = j;
    }
  }
  printf("Most negative column in row[0] is col %d = %g.\n", pivot_col, lowest);
  if( lowest >= 0 ) {
    return -1; // All positive columns in row[0], this is optimal.
  }
  return pivot_col;
}

// Find the pivot_row, with smallest positive ratio = col[0] / col[pivot]
int find_pivot_row(Tableau *tab, int pivot_col) {
  int i, pivot_row = 0;
  double min_ratio = -1;
  printf("Ratios A[row_i,0]/A[row_i,%d] = [",pivot_col);
  for(i=1;i<tab->m;i++){
    double ratio = tab->mat[i][0] / tab->mat[i][pivot_col];
    printf("%3.2lf, ", ratio);
    if ( (ratio > 0  && ratio < min_ratio ) || min_ratio < 0 ) {
      min_ratio = ratio;
      pivot_row = i;
    }
  }
  printf("].\n");
  if (min_ratio == -1)
    return -1; // Unbounded.
  printf("Found pivot A[%d,%d], min positive ratio=%g in row=%d.\n",
      pivot_row, pivot_col, min_ratio, pivot_row);
  return pivot_row;
}

void add_slack_variables(Tableau *tab) {
  int i, j;
  for(i=1; i<tab->m; i++) {
    for(j=1; j<tab->m; j++)
      tab->mat[i][j + tab->n -1] = (i==j);
  }
  tab->n += tab->m -1;
}

void check_b_positive(Tableau *tab) {
  int i;
  for(i=1; i<tab->m; i++)
    assert(tab->mat[i][0] >= 0);
}

// Given a column of identity matrix, find the row containing 1.
// return -1, if the column as not from an identity matrix.
int find_basis_variable(Tableau *tab, int col) {
  int i, xi=-1;
  for(i=1; i < tab->m; i++) {
    if (equal( tab->mat[i][col],1) ) {
      if (xi == -1)
        xi=i;   // found first '1', save this row number.
      else
        return -1; // found second '1', not an identity matrix.

    } else if (!equal( tab->mat[i][col],0) ) {
      return -1; // not an identity matrix column.
    }
  }
  return xi;
}

void print_optimal_vector(Tableau *tab, char *message) {
  int j, xi;
  printf("%s at ", message);
  for(j=1;j<tab->n;j++) { // for each column.
    xi = find_basis_variable(tab, j);
    if (xi != -1)
      printf("x%d=%3.2lf, ", j, tab->mat[xi][0] );
    else
      printf("x%d=0, ", j);
  }
  printf("\n");
} 

void simplex(Tableau *tab) {
  int loop=0;
  add_slack_variables(tab);
  check_b_positive(tab);
  print_tableau(tab,"Padded with slack variables");
  while( ++loop ) {
    int pivot_col, pivot_row;

    pivot_col = find_pivot_column(tab);
    if( pivot_col < 0 ) {
      printf("Found optimal value=A[0,0]=%3.2lf (no negatives in row 0).\n",
        tab->mat[0][0]);
      print_optimal_vector(tab, "Optimal vector");
      break;
    }
    printf("Entering variable x%d to be made basic, so pivot_col=%d.\n",
      pivot_col, pivot_col);

    pivot_row = find_pivot_row(tab, pivot_col);
    if (pivot_row < 0) {
      printf("unbounded (no pivot_row).\n");
      break;
    }
    printf("Leaving variable x%d, so pivot_row=%d\n", pivot_row, pivot_row);

    pivot_on(tab, pivot_row, pivot_col);
    print_tableau(tab,"After pivoting");
    print_optimal_vector(tab, "Basic feasible solution");

    if(loop > 20) {
      printf("Too many iterations > %d.\n", loop);
      break;
    }
  }
}
*/
int main(int argc, char *argv[]) {
    Tableau tab;
    if (argc < 2) {
        cerr << "not enough argument" << endl;
        return 1;
    }
    read_tab(tab, argv[1]);
    print_tab(tab,"Initial");
    return 0;
} 
