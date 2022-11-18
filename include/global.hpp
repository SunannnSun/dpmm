#pragma once

#include <iostream>
#include <Eigen/Dense>
#include <vector>


using namespace std;

typedef Eigen::Matrix<uint32_t,Eigen::Dynamic,1> VectorXu;
typedef Eigen::Matrix<uint32_t,Eigen::Dynamic,Eigen::Dynamic> MatrixXu;

#ifndef PI
#  define PI 3.141592653589793
#endif
#define LOG_PI 1.1447298858494002
#define LOG_2 0.69314718055994529
#define LOG_2PI 1.8378770664093453



// int getNum(vector<int>& v)
// {
 
//     // Size of the vector
//     int n = v.size();
 
//     // Generate a random number
//     srand(time(NULL));
 
//     // Make sure the number is within
//     // the index range
//     int index = rand() % n;
 
//     // Get random number from the vector
//     int num = v[index];
 
//     // Remove the number from the vector
//     swap(v[index], v[n - 1]);
//     v.pop_back();
 
//     // Return the removed number
//     return num;
// }
 
// // Function to generate n non-repeating random numbers
// void generateRandom(int n)
// {
//     vector<int> v(n);
 
//     // Fill the vector with the values
//     // 1, 2, 3, ..., n
//     for (int i = 0; i < n; i++)
//         v[i] = i + 1;
 
//     // While vector has elements
//     // get a random number from the vector and print it
//     while (v.size()) {
//         cout << getNum(v) << " ";
//     }
// }