#include "niw.hpp"

// template<typename T>
// NIW<T>::NIW(){};


template<typename T>
NIW<T>::NIW(const Matrix<T,Dynamic,Dynamic>& sigma, 
  const Matrix<T,Dynamic,Dynamic>& mu, T nu,  T kappa, boost::mt19937 *pRndGen)
: Distribution<T>(pRndGen), sigma_(sigma), mu_(mu), nu_(nu), kappa_(kappa), dim_(mu.size())
{
  assert(sigma_.rows()==mu_.size()); 
  assert(sigma_.cols()==mu_.size());
};

template<typename T>
NIW<T>::~NIW()
{};


template<typename T>
T NIW<T>::logPosteriorProb(const Vector<T,Dynamic>& x_i, const Matrix<T,Dynamic, Dynamic>& x_k)
{
  NIW<T> posterior = this ->posterior(x_k);
  return posterior.logProb(x_i);
  // return x_i.rows() + x_k.cols();
};

template<typename T>
NIW<T> NIW<T>::posterior(const Matrix<T,Dynamic, Dynamic>& x_k)
{
  getSufficientStatistics(x_k);
  return NIW<T>(
    sigma_+scatter_ + ((kappa_*count_)/(kappa_+count_))
      *(mean_-mu_)*(mean_-mu_).transpose(), 
    (kappa_*mu_+ count_*mean_)/(kappa_+count_),
    nu_+count_,
    kappa_+count_,
    this->pRndGen_);
};

template<typename T>
void NIW<T>::getSufficientStatistics(const Matrix<T,Dynamic, Dynamic>& x_k)
{
	mean_ = x_k.colwise().mean();
  // cout << mean <<endl;
  Matrix<T,Dynamic, Dynamic> x_k_mean;
  // MatrixXd x_k
  x_k_mean = x_k.rowwise() - mean_.transpose();
  // x_k_mean = x_k.rowwise() - x_k.colwise().mean();

  scatter_ = x_k_mean.adjoint() * x_k_mean;
	count_ = x_k.rows();
};

template<typename T>
T NIW<T>::logProb(const Matrix<T,Dynamic,1>& x_i)
{
  // std::cout << x_i << std::endl;
  // using multivariate student-t distribution; missing terms?
  // https://en.wikipedia.org/wiki/Multivariate_t-distribution
  // https://www.cs.ubc.ca/~murphyk/Papers/bayesGauss.pdf pg.21
  T doF = nu_ - dim_ + 1.;
  Matrix<T,Dynamic,Dynamic> scaledSigma = sigma_*(kappa_+1.)/(kappa_*(nu_-dim_+1));   
  // scaledSigma(dim_-1, dim_-1) = sigma_(dim_-1, dim_-1); //testing the z-value effects if all zeros
                    
  T logProb = boost::math::lgamma(0.5*(doF + dim_));
  logProb -= boost::math::lgamma(0.5*(doF));
  logProb -= 0.5*dim_*log(doF);
  logProb -= 0.5*dim_*log(PI);
  logProb -= 0.5*log(scaledSigma.determinant());
  // logProb -= 0.5*((scaledSigma.eigenvalues()).array().log().sum()).real();
  logProb -= (0.5*(doF + dim_))
    *log(1.+ 1/doF*((x_i-mu_).transpose()*scaledSigma.inverse()*(x_i-mu_)).sum());
  // approximate using moment-matched Gaussian; Erik Sudderth PhD essay
  return logProb;
};


// template<typename T>
// T NIW<T>::logPosteriorProb(const Matrix<T,Dynamic,Dynamic>& x, VectorXu& z, uint32_t k, uint32_t i)
// {
//   uint32_t z_i = z[i];
//   z[i] = k+1; // so that we definitely not use x_i in posterior computation 
//   // (since the posterior is only computed form x_{z==k})
//   NIW posterior = this->posterior(x,z,k);
//   z[i] = z_i; // reset to old value
//   return posterior.logProb(x.col(i));
// };