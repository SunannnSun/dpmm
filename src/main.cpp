// This file is not used, and only serves testing purpose
#include <iostream>
#include <Eigen/Core>
#include <limits>
#include "karcher.hpp"
#include "global.hpp"
#include "permutationArray.hpp"


using namespace std;
using namespace Eigen;

int main ()
{
    // Testing block for calculating posterior prob for cluster whose number is zero...log(0) should be negative infinity 
    // int K_ = 10;
    // int kk = 5;
    // VectorXd pi(K_); 
    // cout << pi << endl;
    // for (int i=0; i<K_; ++i)
    // {
    //     if (i!=kk)
    //     pi(i) = 1;
    //     else
    //     pi(i) = - std::numeric_limits<float>::infinity();
    // }
    // cout << pi << endl;
    // double pi_max = pi.maxCoeff();
    // pi = (pi.array()-(pi_max + log((pi.array() - pi_max).exp().sum()))).exp().matrix();
    // pi = pi / pi.sum();
    // cout << pi << endl;

    // int n = 20;
    // for (auto const& c :   generateRandom(n))
    // std::cout << c << ' ';

    
    MatrixXd a {{1, 2, cos(PI/2), sin(PI/2)}, {1, 2, cos(PI/2), sin(PI/2)},};
    MatrixXd b {{1 ,2, cos(0), sin(0)}};
    cout << karcherMean(a) << endl;
    cout << karcherScatter(a, karcherMean(b)) << endl;
    cout << karcherVariance(a, karcherMean(b));


    return 0;
}