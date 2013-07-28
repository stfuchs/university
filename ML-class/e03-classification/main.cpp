#include <iostream>
#include <limits>
#include <algorithm>

#include <boost/program_options.hpp>

#include <MLUtils/io.h>
#include <MLUtils/features.h>

std::string file_in_;
std::string file_label_;
double lambda_;
int mode_;

int readOptions(int argc, char** argv)
{
  using namespace boost::program_options;
  options_description options("Options");
  options.add_options()
    ("help,h", "produce help message")
    ("in,x", value<std::string>(&file_in_), "input file with data points")
    ("label,y", value<std::string>(&file_label_), "optional file containing labels")
    ("lambda,l", value<double>(&lambda_)->default_value(1.0), "lambda")
    ("mode,m", value<int>(&mode_)->default_value(1), "mode 1: linear\nmode 2: quadratic")
    ;

  positional_options_description p_opt;
  p_opt.add("in", 1);
  variables_map vm;
  store(command_line_parser(argc, argv).options(options).positional(p_opt).run(), vm);
  notify(vm);

  if(vm.count("help") || argc == 1)
  { std::cout << options << std::endl; return(-1); }
  if(file_in_ == "")
  { std::cout << "no input file defined!\n" << options << std::endl; return(-1); }
  if (mode_ != 1 && mode_ != 2)
  { std::cout << "Don't know this mode: " << mode_ << "\n" << options << std::endl; return(-1); }
}

int main(int argc, char** argv)
{
  if(readOptions(argc, argv) == -1) return 0;

  Eigen::MatrixXd Y, X;
  Eigen::MatrixXd input;
  MLUtils::read(file_in_, input);

  int n = input.rows(); // data points
  int d = input.cols(); // dimension

  if(file_label_ != "") { MLUtils::read(file_label_, Y); }
  else { d -= 1; Y = input.col(d); }

  switch(mode_)
  {
    case 1:
    {
      std::cout << "Features: Linear" << std::endl;
      MLUtils::fillFeatureMatrixLin(input, d, X);
      break;
    }
    case 2:
    {
      std::cout << "Features: Quadratic" << std::endl;
      MLUtils::fillFeatureMatrixQuad(input, d, X);
      break;
    }
  }
  std::cout << "Lambda: " << lambda_ << "\n\nStart training..." << std::endl;

  Eigen::MatrixXd Xt = X.transpose();
  int k = X.cols(); // feature size

  Eigen::VectorXd* beta_old = new Eigen::VectorXd(k);
  Eigen::VectorXd* beta_new = new Eigen::VectorXd(k);
  Eigen::MatrixXd W = Eigen::MatrixXd::Zero(n, n);
  Eigen::MatrixXd H = Eigen::MatrixXd(k, k);
  int iter = 0;
  double eps = std::numeric_limits<double>::max();
  Eigen::MatrixXd lambda_matrix = 2.0 * lambda_ * Eigen::MatrixXd::Identity(k,k); // lambda matrix: 2*lambda*I

  *beta_old = Eigen::VectorXd::Constant(k, 0.0);
  while(eps > 0.00001 && iter < 100)
  {
    Eigen::VectorXd prop(n); // vector of fitted probabilities for beta_old
    for(int i=0; i<n; ++i)
    {
      prop(i) = MLUtils::conditionalClassProbability(X.row(i), *beta_old);
      W(i,i) = prop(i) * ( 1-prop(i) ); // fill diagonal matrix of weights
    }
    H = Xt * W * X + lambda_matrix; // Hessian matrix
    *beta_new = *beta_old - (H.inverse() * (Xt * (prop - Y) + lambda_matrix * *beta_old)); // Newton step
    eps = (*beta_old - *beta_new).squaredNorm();
    std::cout << "Epsilon: " << eps << std::endl;
    std::swap(beta_old, beta_new);
    std::cout << "neg-log-likelihood: " << MLUtils::computeNegLogLikelihood(X,Y,*beta_old, 1.0) << std::endl;
    ++iter;
  }

  std::cout << "Finished within " << iter << " iterations" << std::endl;
  std::cout << "\nBeta:\n" << MLUtils::betaToString(*beta_old) << std::endl;
  std::cout << "mean neg-log-likelihood: " << MLUtils::computeNegLogLikelihood(X,Y,*beta_old, lambda_) / X.rows() << std::endl;
  std::cout << "correct classification rate: " << MLUtils::computeClassificationRate(X,Y,*beta_old) * 100.0 <<"%" << std::endl;

  delete beta_old;
  delete beta_new;

  return 0;
}
