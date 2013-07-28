#include <iostream>
#include <MLUtils/io.h>
#include <MLUtils/features.h>

int main(int argc, char** argv)
{
  std::string name(argv[1]);
  Eigen::MatrixXd input;
  MLUtils::read(name, input);
  int n = input.rows();
  int d = input.cols() - 1;
  Eigen::MatrixXd Y = input.col(d);
  Eigen::MatrixXd X;
  if( argc > 2 && std::string(argv[2]) == "m2" )
  {
    X = Eigen::MatrixXd(n, 1+d + int(0.5*d*(d+1)));
    for(int i=0; i<n; ++i)
      X.row(i) = MLUtils::quadraticFeature(input.block(i,0,1,d));
  }
  else
  {
    X = Eigen::MatrixXd(n, 1+d);
    for(int i=0; i<n; ++i)
      X.row(i) = MLUtils::linearFeature(input.block(i,0,1,d));
  }

  //std::cout << X << std::endl;

  Eigen::VectorXd beta = (X.transpose() * X).inverse() * X.transpose() * Y;
  std::cout << "\nbeta:\n" << beta << std::endl;

// Uncomment for prediction test:
/*
  Eigen::MatrixXd Ztmp = Eigen::MatrixXd::Zero(3, 2);
  Ztmp << 1.0, 1.5,
    2.75, 3.0,
    0.5, 2.5;

  Eigen::MatrixXd Z = Eigen::MatrixXd(3, 1+d + int(0.5*d*(d+1)));
  for(int i=0; i<Ztmp.rows(); ++i)
    Z.row(i) = MLUtils::quadraticFeature(Ztmp.row(i));

  Eigen::VectorXd pred = Z*beta;

  std::cout << "\nx/y:\n" << Ztmp << std::endl;
  std::cout << "\npred:\n" << pred << std::endl;
*/
}
