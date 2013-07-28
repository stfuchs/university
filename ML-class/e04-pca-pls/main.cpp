#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <MLUtils/io.h>
#include <MLUtils/features.h>

std::string folder_in_;
std::string folder_out_;
int p;

int readOptions(int argc, char** argv)
{
  using namespace boost::program_options;
  options_description options("Options");
  options.add_options()
    ("help,h", "produce help message")
    ("in,i", value<std::string>(&folder_in_), "input folder with data points")
    ("out,o", value<std::string>(&folder_out_), "out file with data points")
    ("p,p", value<int>(&p)->default_value(1), "p")
    ;

  positional_options_description p_opt;
  p_opt.add("in", 1);
  variables_map vm;
  store(command_line_parser(argc, argv).options(options).positional(p_opt).run(), vm);
  notify(vm);

  if(vm.count("help") || argc == 1)
  { std::cout << options << std::endl; return(-1); }
  if(folder_in_ == "")
  { std::cout << "no input file defined!\n" << options << std::endl; return(-1); }
}

int main(int argc, char** argv)
{
  readOptions(argc,argv);

  std::vector<std::string> filenames;
  MLUtils::getAllFilesInFolder(folder_in_, ".pgm", filenames);

  Eigen::MatrixXd mat;
  MLUtils::ImageHeader header;
  std::vector<float> values;

  if(readImage(filenames[0], header, values)) exit(-1);
  std::cout << "Loaded Image 0 ... " << header.print() << std::endl;
  mat.resize(filenames.size(),values.size());
  float norm = 1.0f / float(header.max_val);
  for(int i=0; i<header.rows; ++i) { mat(0,i) = values[i] * norm; }

  for(int f=1; f<filenames.size(); ++f)
  {
    values.clear();
    if(readImage(filenames[f], header, values)) exit(-1);
    std::cout << "Loaded Image "<<f<<" ... " << header.print() << std::endl;
    norm = 1.0f / float(header.max_val);
    for(int i=0; i<values.size(); ++i) { mat(f,i) = values[i] * norm; }
  }

  int n = mat.rows();
  int d = mat.cols();
  std::cout << n <<"x"<< d << std::endl;
  Eigen::RowVectorXd v_mean = Eigen::RowVectorXd::Zero(d);
  for(int i=0; i<n; ++i)
  {
    v_mean += mat.row(i);
  }
  v_mean / float(n);


  mat = mat - Eigen::VectorXd::Constant(n,1) * v_mean;
  //MLUtils::writeImage(folder_out_,mat);

  std::cout << "start" << std::endl;
  Eigen::JacobiSVD<Eigen::MatrixXd> svd(mat, Eigen::ComputeThinV);
  std::cout << "init" << std::endl;
  Eigen::MatrixXd V = svd.matrixV();
  Eigen::MatrixXd Z = mat * V.leftCols(p);
  Eigen::MatrixXd X = Eigen::VectorXd::Constant(n,1) * v_mean + Z*V.leftCols(p).transpose();

  char* buffer = new char[d];
  for(int r=0; r<n; ++r)
  {
    float max=0, min=1000.0;
    for(int c=0; c<d; ++c)
    {
      max = std::max(float(X(r,c)), max);
      min = std::min(float(X(r,c)), min);
    }
    for(int c=0; c<d; ++c)
    {
      buffer[c] = (X(r,c) - min)/(max-min) * 255.0f;
    }
    std::stringstream ss;
    ss << folder_out_ << "recon"<<r<<".pgm";
    MLUtils::writeImage(ss.str(), header, buffer, mat.cols());
  
    std::cout << "max: " << max << " min: " << min << std::endl;
  }


  
  
  delete[] buffer;

  //if(file_out_ != "") MLUtils::writeImage(file_out_,mat);

}
