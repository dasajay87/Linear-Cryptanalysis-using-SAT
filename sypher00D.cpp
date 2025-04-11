//SAT encoding for generating Linear trail of Sypher00D

#include <cryptominisat5/cryptominisat.h>
#include <assert.h>
#include <vector>
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>

using std::vector;
using namespace CMSat;
using namespace std;

//Best weight for bias
int BestWeight = 6;

//Constraints for modeling LAT of Gift - Sbox
int Constraint[31][10] = {
    {9, 9, 0, 0, 9, 9, 9, 9, 1, 9},
    {9, 9, 9, 0, 0, 9, 9, 9, 1, 9},
    {9, 9, 0, 9, 9, 0, 9, 9, 1, 9},
    {9, 9, 9, 9, 0, 0, 9, 9, 1, 9},
    {9, 9, 9, 1, 0, 1, 9, 9, 0, 9},
    {9, 9, 1, 1, 1, 1, 9, 9, 1, 9},
    {9, 1, 9, 9, 9, 9, 9, 9, 9, 0},
    {9, 9, 9, 9, 1, 9, 9, 9, 9, 0},
    {9, 9, 9, 9, 9, 1, 9, 9, 9, 0},
    {9, 9, 9, 9, 9, 9, 1, 9, 9, 0},
    {9, 9, 1, 0, 1, 9, 9, 9, 0, 9},
    {9, 9, 0, 1, 9, 1, 9, 9, 0, 9},
    {9, 9, 1, 9, 1, 0, 9, 9, 0, 9},
    {0, 1, 0, 0, 9, 0, 9, 9, 9, 9},
    {1, 0, 0, 0, 9, 0, 9, 9, 9, 9},
    {9, 1, 0, 9, 9, 0, 1, 1, 9, 9},
    {9, 0, 0, 1, 0, 9, 9, 9, 0, 9},
    {1, 1, 1, 9, 9, 9, 1, 9, 0, 9},
    {1, 1, 9, 9, 9, 1, 1, 9, 0, 9},
    {0, 9, 1, 9, 9, 1, 1, 9, 0, 9},
    {1, 0, 1, 9, 9, 9, 9, 1, 0, 9},
    {9, 1, 9, 9, 9, 1, 0, 1, 0, 9},
    {9, 0, 9, 9, 9, 0, 0, 1, 0, 9},
    {9, 0, 0, 9, 9, 9, 1, 0, 0, 9},
    {9, 0, 1, 9, 9, 9, 0, 0, 0, 9},
    {9, 1, 9, 9, 9, 0, 0, 0, 0, 9},
    {0, 0, 0, 0, 9, 9, 9, 9, 9, 1},
    {0, 9, 9, 9, 0, 0, 9, 0, 9, 1},
    {9, 9, 0, 0, 9, 9, 0, 0, 9, 1},
    {1, 1, 9, 1, 1, 0, 9, 9, 0, 9},
    {0, 1, 9, 9, 0, 9, 0, 1, 0, 9}
};

//Code for SAT modeling Sum(x_i) <= w
void GenerateLess(int * x, int ** u, int * vn, int * cn,
    SATSolver & solver, vector<Lit> & clause)
{
    int n=(*vn);
    int k=(*cn);

    if (k>0)
    {
        clause.clear();
        clause.push_back(Lit(x[0], true));
        clause.push_back(Lit(u[0][0],false));
        solver.add_clause(clause);

        for (int j=1;j<k;j++)
        {
            clause.clear();
            clause.push_back(Lit(u[0][j],true));
            solver.add_clause(clause);
        }
        for (int i=1;i<n-1;i++)
        {
            clause.clear();
            clause.push_back(Lit(x[i],true));
            clause.push_back(Lit(u[i][0],false));
            solver.add_clause(clause);

            clause.clear();
            clause.push_back(Lit(u[i-1][0],true));
            clause.push_back(Lit(u[i][0],false));
            solver.add_clause(clause);

            clause.clear();
            clause.push_back(Lit(x[i],true));
            clause.push_back(Lit(u[i-1][k-1],true));
            solver.add_clause(clause);
        }
        for (int j=1;j<k;j++)
        {
            for (int i=1;i<n-1;i++)
            {
                clause.clear();
                clause.push_back(Lit(x[i],true));
                clause.push_back(Lit(u[i-1][j-1],true));
                clause.push_back(Lit(u[i][j],false));
                solver.add_clause(clause);

                clause.clear();
                clause.push_back(Lit(u[i-1][j],true));
                clause.push_back(Lit(u[i][j],false));
                solver.add_clause(clause);
            }
        }

        clause.clear();
        clause.push_back(Lit(x[n-1],true));
        clause.push_back(Lit(u[n-2][k-1],true));
        solver.add_clause(clause);
    }
    if (k==0)
    {
        for (int i=0;i<n;i++)
        {
            clause.clear();
            clause.push_back(Lit(x[i], true));
            solver.add_clause(clause);
        }
    }
}

int main()
{
    int trailround = 5;
    for (int trailweight = BestWeight; trailweight < BestWeight + 1; trailweight++)
    {
        int ** xin = new int * [trailround];
        int ** p = new int * [trailround];
        int ** q = new int * [trailround];
        int ** m = new int * [trailround];
        int ** w = new int * [trailround];
        int ** xout = new int * [trailround];

        for (int round = 0; round < trailround; round++)
        {
            xin[round] = new int [16];
            p[round] = new int [4];
            q[round] = new int [4];
            xout[round] = new int [16];
        }

        // Allocate variable
        int sx = 0;
        for (int round = 0; round < trailround; round++)
        {
            for (int i = 0; i < 16; i++)
            {
                xin[round][i] = sx++;
            }
            for (int i = 0; i < 4; i++)
            {
                p[round][i] = sx++;
            }
            for (int i = 0; i < 4; i++)
            {
                q[round][i] = sx++;
            }
        }

        for (int round = 0; round < trailround - 1; round++)
        {
            for (int i = 0; i < 16; i++)
            {
                xout[round][i] = xin[round + 1][i];
            }
        }
        for (int i = 0; i < 16; i++)
        {
            xout[trailround - 1][i] = sx++;
        }

	//Total prob in terms of Bias (#Sbox * #round * #var_for_encoding_weight)
        int TotalProb = 4 * trailround * 2;
        int Prob = trailweight;

        int ** UP = new int * [TotalProb - 1];
        for (int i = 0; i < TotalProb - 1; i++)
        {
            UP[i] = new int [Prob];
        }
        for (int i = 0; i < TotalProb - 1; i++)
        {
            for (int j = 0; j < Prob; j++)
            {
                UP[i][j] = sx++;
            }
        }

        SATSolver solver;
        vector<Lit> clause;
        solver.set_num_threads(1);
        solver.new_vars(sx);

        // Nonzero Input Mask
        clause.clear();
        for (int i=0;i<16;i++)
        {
            clause.push_back(Lit(xin[0][i],false));
        }
        solver.add_clause(clause);

        for (int round = 0; round < trailround; round++)
        {
            int y[16];
            int P[16] = {0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15};
            for (int i = 0; i < 16; i++)
            {
                y[i] = xout[round][P[i]];
            }

            for (int sbox = 0; sbox < 4; sbox++)
            {
                int X[10];
                for (int i = 0; i < 4; i++)
                {
                    X[i] = xin[round][4 * sbox + i];
                    X[i + 4] = y[4 * sbox + i];
                }
                X[8] = p[round][sbox];
                X[9] = q[round][sbox];
                for (int restriction = 0; restriction < 31; restriction++)
                {
                    clause.clear();
                    for (int i = 0; i < 10; i++)
                    {
                        if (Constraint[restriction][i] == 1)
                        {
                            clause.push_back(Lit(X[i], true));
                        }
                        if (Constraint[restriction][i] == 0)
                        {
                            clause.push_back(Lit(X[i], false));
                        }
                    }
                    solver.add_clause(clause);
                }
            }
        }


        int * AddProb = new int [TotalProb];
        for (int round = 0; round < trailround; round++)
        {
            for (int i = 0; i < 4; i++)
            {
                AddProb[8 * round + i] = p[round][i];
                AddProb[8 * round + i + 4] = q[round][i];
            }
        }

        int NumProb = TotalProb;
        int ConsProb = Prob;

        GenerateLess(AddProb, UP, &NumProb, &ConsProb, solver, clause);

        lbool ret = solver.solve();

        if (ret == l_True)
        {
            for (size_t round = 0; round < trailround; round++)
            {
                cout<<"Round: "<<(dec)<<round<<" ---------------------------- "<<"\n";
                int tem[16];
                cout<<"xin: "<<"\n";
                for (size_t i = 0; i < 16; i++)
                {
                    if (solver.get_model()[xin[round][i]]!=l_Undef)
                    {
                        if (solver.get_model()[xin[round][i]]==l_True)
                        {
                            tem[i]=1;
                        }
                        else
                        {
                            tem[i]=0;
                        }
                    }
                }
                    for (int sbox = 0; sbox < 4; sbox++)
                {
                    int a = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        a ^= (tem[4 * sbox + i] << (3 - i));
                    }
                    cout<<(hex)<<a<<",";
                    if (((sbox + 1)%4) == 0)
                    {
                        cout<<" ";
                    }
                }
                cout<<"\n";

                cout<<"xout: "<<"\n";
                for (size_t i = 0; i < 16; i++)
                {
                    if (solver.get_model()[xout[round][i]]!=l_Undef)
                    {
                        if (solver.get_model()[xout[round][i]]==l_True)
                        {
                            tem[i]=1;
                        }
                        else
                        {
                            tem[i]=0;
                        }
                    }
                }
                 for (int sbox = 0; sbox < 4; sbox++)
                {
                    int a = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        a ^= (tem[4 * sbox + i] << (3 - i));
                    }
                    cout<<(hex)<<a<<",";
                    if (((sbox + 1)%4) == 0)
                    {
                        cout<<" ";
                    }
                }
                cout<<"\n";
            }
        }
	else
	{
		cout << "UNSAT on given Bias : " << BestWeight << "\n";
	}

        for (int round = 0; round < trailround; round++)
        {
            delete [] xin[round];
            delete [] p[round];
            delete [] q[round];
            delete [] xout[round];
        }
        delete [] xin;
        delete [] p;
        delete [] q;
        delete [] xout;
        for (int i = 0; i < TotalProb - 1; i++)
        {
            delete [] UP[i];
        }
        delete [] UP;
        delete [] AddProb;
    }
    return 0;
}

