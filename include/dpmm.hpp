#pragma once

#include <iostream>
#include <limits>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <Eigen/Dense>

#include "niw.hpp"
#include "global.hpp"
#include "permutationArray.hpp"

using namespace Eigen;
using namespace std;


/*
 * DP mixture model
 * following Neal [[http://www.stat.purdue.edu/~rdutta/24.PDF]]
 * Algo 3
 */

template <class Dist_t>
class DPMM
{
public:
  // DPMM(){};
  DPMM(double alpha, const Dist_t& H): alpha_(alpha), H_(H) {};
  ~DPMM(){};

  void initialize(const MatrixXd& x, int init_cluster);
  void sampleLabels();
  void reorderAssignments();
  const VectorXi & getLabels(){return z_;};

public:
  double alpha_; 
  Dist_t H_; 
  MatrixXd x_;
  VectorXi z_; 
  uint16_t N_;
  uint16_t K_;
  vector<Dist_t> components_;
};

// ---------------- impl -----------------------------------------------------
template <class Dist_t> 
void DPMM<Dist_t>::initialize(const MatrixXd& x, int init_cluster)
{
  x_ = x;
  N_ = x_.rows();
  
  VectorXi z(x.rows());
  if (init_cluster == 1) 
  {
    z.setZero();
  }
  else if (init_cluster==0)
  {
    vector<int> zz;
    for (int i=0; i<N_; ++i) zz.push_back(i);
    random_shuffle(zz.begin(), zz.end());
    for (int i=0; i<N_; ++i)z[i] =zz[i];
    // cout << z_ << endl;
  }
  else if (init_cluster >= 1)
  {
    boost::random::uniform_int_distribution<> uni_(0, init_cluster-1);
    for (int i=0; i<N_; ++i)z[i] = uni_(*H_.pRndGen_); 
  }
  else
  { 
    cout<< "Number of initial clusters not supported yet" << endl;
    exit(1);
  }
  z_ = z;
  K_ = z_.maxCoeff() + 1; //=1


  for (uint32_t k=0; k<K_; ++k)
    components_.push_back(H_);
    // components_.push_back(Dist_t(H_));
  cout<< "Initial clusters: " << components_.size()<<endl;

  //random number of initial components; and sample each assignment from a symmetric categorical distribution
  // if (K0>1)
  // {
  //   VectorXd pi(K0);
  //   pi.setOnes();
  //   pi /= static_cast<double>(K0);
  //   Catd cat(pi,H_.pRndGen_);
  //   for (uint32_t i=0; i<z_.size(); ++i)
  //     z_(i) = cat.sample();
  // }
};


template <class Dist_t> 
void DPMM<Dist_t>::sampleLabels()
{
  vector<int> i_array = generateRandom(N_);
  for(uint32_t j=0; j<N_; ++j)
  // for(uint32_t i=0; i<N_; ++i)
  {
    int i = i_array[j];
    // cout << "number of data point: " << i << endl;
    VectorXi Nk(K_);
    Nk.setZero();
    for(uint32_t ii=0; ii<N_; ++ii)
    {
      if (ii != i) Nk(z_(ii))++;
    }
    // cout<< Nk << endl;
    VectorXd pi(K_+1); 
    // VectorXd pi(K_); 

    VectorXd x_i;
    x_i = x_(i, all); //current data point x_i

    // #pragma omp parallel for
    for (uint32_t k=0; k<K_; ++k)
    { 
      vector<int> x_k_index;
      for (uint32_t ii = 0; ii<N_; ++ii)
      {
        if (ii!= i && z_[ii] == k) x_k_index.push_back(ii); 
      }
      // if (x_k_index.empty()) 
      // cout << "no index" << endl;
      // cout << "im here" <<endl;


      MatrixXd x_k(x_k_index.size(), x_.cols()); 
      x_k = x_(x_k_index, all);
      // cout << "x_i" << x_i << endl;
      // cout << "x_k" << x_k << endl;
      // cout << "component:" <<k  <<endl;
      // cout << x_k << endl;
      // cout << Nk(k) << endl;
      if (Nk(k)!=0)
      pi(k) = log(Nk(k))-log(N_+alpha_) + components_[k].logPosteriorProb(x_i, x_k);
      else
      pi(k) = - std::numeric_limits<float>::infinity();
    }
    pi(K_) = log(alpha_)-log(N_+alpha_) + H_.logProb(x_i);


    // cout << pi <<endl;
    // exit(1);


    
    
    double pi_max = pi.maxCoeff();
    pi = (pi.array()-(pi_max + log((pi.array() - pi_max).exp().sum()))).exp().matrix();
    pi = pi / pi.sum();


    for (uint32_t ii = 1; ii < pi.size(); ++ii){
      pi[ii] = pi[ii-1]+ pi[ii];
    }
   
    boost::random::uniform_01<> uni_;   
    boost::random::variate_generator<boost::random::mt19937&, 
                           boost::random::uniform_01<> > var_nor(*H_.pRndGen_, uni_);
    double uni_draw = var_nor();
    uint32_t k = 0;
    while (pi[k] < uni_draw) k++;
    z_[i] = k;
    this -> reorderAssignments();
  }

};


template <class Dist_t>
void DPMM<Dist_t>::reorderAssignments()
{ 
  // cout << z_ << endl;
  vector<uint8_t> rearrange_list;
  for (uint32_t i=0; i<N_; ++i)
  {
    if (rearrange_list.empty()) rearrange_list.push_back(z_[i]);
    // cout << *rearrange_list.begin() << endl;
    // cout << *rearrange_list.end()  << endl;
    std::vector<uint8_t>::iterator it;
    it = find (rearrange_list.begin(), rearrange_list.end(), z_[i]);
    if (it == rearrange_list.end())
    {
      rearrange_list.push_back(z_[i]);
      // z_[i] = rearrange_list.end() - rearrange_list.begin();
      z_[i] = rearrange_list.size() - 1;
    }
    else if (it != rearrange_list.end())
    {
      int index = it - rearrange_list.begin();
      z_[i] = index;
    }
  }
  K_ = z_.maxCoeff() + 1;
  if(K_>components_.size())
  components_.push_back(H_);
  else if(K_<components_.size())
  components_.pop_back();
    // std::cout << "Element found in myvector: " << *it << '\n';
    // else
    // std::cout << "Element not found in myvector\n";
}
    // cout << z_ << endl;




// template <class Dist_t>
// void DPMM<Dist_t>::removeEmptyClusters()
// {
//   for(uint32_t k=components_.size()-1; k>=0; --k)
//   {
//     bool haveCluster_k = false;
//     for(uint32_t i=0; i<z_.size(); ++i)
//       if(z_(i)==k)
//       {
//         haveCluster_k = true;
//         break;
//       }
//     if (!haveCluster_k)
//     {
//       for (uint32_t i=0; i<z_.size(); ++i)
//         if(z_(i) >= k) z_(i) --;
//       components_.erase(components_.begin()+k);
//     }
//   }
// }

