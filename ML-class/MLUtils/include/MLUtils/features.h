#include <Eigen/Dense>

namespace MLUtils
{
  /*
    0:  1,   x0,   x1,   x2,   x3;
    5:     x0x0, x0x1, x0x2, x0x3;
    9:           x1x1, x1x2, x1x3;
    12:                x2x2, x2x3;
    14:                      x3x3;


    (0,0) = 0 (0,1) = 1 (0,2) = 2  -(0+0)
    (1,1) = 3 (1,2) = 4  -(0+1)
    (2,2) = 5  -(1+2)

    (0,0) = 0 (0,1) = 1 (0,2) = 2 (0,3) = 3 -(0+0)
    (1,1) = 4 (1,2) = 5 (1,3) = 6 -(0+1)
    (2,2) = 7 (2,3) = 8 -(1+2)
    (3,3) = 9 -(3+3)
  */

  Eigen::RowVectorXd linearFeature(const Eigen::RowVectorXd& input)
  {
    Eigen::RowVectorXd m(1,1+input.cols());
    m(0,0) = 1.0;
    m.rightCols(input.cols()) = input;
    return m;
  }

  Eigen::RowVectorXd quadraticFeature(const Eigen::RowVectorXd& input)
  {
    int d = input.cols();
    int idx = d + int(0.5*d*(d+1));
    Eigen::RowVectorXd m(1, idx+1);
    m.leftCols(d+1) = linearFeature(input);

    int inv_idx = 0;
    for(int i=1; i<=d; ++i)
    {
      inv_idx += i;
      for(int j=i; j<=d; ++j)
      {
        idx = (d+1)*i + j - inv_idx;
        m(0, idx) = m(0,i) * m(0,j);
      }
    }
    return m;
  }

  void fillFeatureMatrixLin(const Eigen::MatrixXd& data, int dim, Eigen::MatrixXd& out)
  {
    out = Eigen::MatrixXd(data.rows(), 1+dim);
    for(int i=0; i<data.rows(); ++i) { out.row(i) = MLUtils::linearFeature(data.block(i,0,1,dim)); }
  }

  void fillFeatureMatrixQuad(const Eigen::MatrixXd& data, int dim, Eigen::MatrixXd& out)
  {
    out = Eigen::MatrixXd(data.rows(), 1+dim + int(0.5*dim*(dim+1)));
    for(int i=0; i<data.rows(); ++i) { out.row(i) = MLUtils::quadraticFeature(data.block(i,0,1,dim)); }
  }


  // binary case
  // TODO: multi-class case
  double conditionalClassProbability(const Eigen::VectorXd& xi, const Eigen::VectorXd& beta)
  {
    return ( 1.0 / (1.0 + exp( -double(xi.transpose() * beta) )) );
  }

  double computeNegLogLikelihood(const Eigen::MatrixXd& X, const Eigen::MatrixXd& Y, const Eigen::VectorXd& beta, double lambda)
  {
    double res = 0;
    for(int i=0; i<X.rows(); ++i)
    {
      double pi = conditionalClassProbability(X.row(i), beta);
      res += Y(i)*log(pi) + (1.0-Y(i))*log(1.0-pi);
    }
    return (-res + lambda * beta.squaredNorm());
  }

  double computeClassificationRate(const Eigen::MatrixXd& X, const Eigen::MatrixXd& Y, const Eigen::VectorXd& beta)
  {
    int res = 0;
    for(int i=0; i<X.rows(); ++i)
    {
      if( fabs(Y(i) - conditionalClassProbability(X.row(i), beta)) < 0.5 ) ++res;
    }
    return (double(res) / X.rows());
  }
}

