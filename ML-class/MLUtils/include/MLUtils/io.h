#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <Eigen/Dense>

namespace MLUtils
{
  namespace ImageType
  {
    enum types { PLOT, PGMA, PPMA, PGMB, PPMB, number_of_types };
  }

  class TypeInfo
  {
  public:
    struct info {
      bool binary;
      bool color;
      int header_size;
      std::string name;
      std::string magic_number;
    };

    static const info& get(ImageType::types type) { return info_list_[type]; }
    static bool isBinary(ImageType::types type) { return info_list_[type].binary; }
    static bool hasColor(ImageType::types type) { return info_list_[type].color; }
    static int headerSize(ImageType::types type) { return info_list_[type].header_size; }
    static const std::string& getName(ImageType::types type)  { return info_list_[type].name; }
    static const std::string& getMagic(ImageType::types type) { return info_list_[type].magic_number; }

    friend bool operator==(const std::string& lhs, ImageType::types);
    friend bool operator==(ImageType::types lhs, const std::string& rhs);

  private:
    static const info info_list_[ImageType::number_of_types];
  };

  const TypeInfo::info TypeInfo::info_list_[ImageType::number_of_types] =
  {
    { false, false, 0, "Plot"      , ""   }, // Plot
    { false, false, 4, "PGM ASCII" , "P2" }, // PGMA
    { false, true,  4, "PPM ASCII" , "P3" }, // PPMA
    { true , false, 4, "PGM BINARY", "P5" }, // PGMB
    { true , true , 4, "PPM BINARY", "P6" }  // PPMB
  };

  bool operator==(const std::string& lhs, ImageType::types rhs)
  { return (lhs == TypeInfo::info_list_[rhs].magic_number); }

  bool operator==(ImageType::types lhs, const std::string& rhs)
  { return (rhs == TypeInfo::info_list_[lhs].magic_number); }

  ImageType::types stringToType(const std::string& magic_number)
  {
    if(magic_number == ImageType::PGMA) return ImageType::PGMA;
    else if(magic_number == ImageType::PPMA) return ImageType::PPMA;
    else if(magic_number == ImageType::PGMB) return ImageType::PGMB;
    else if(magic_number == ImageType::PPMB) return ImageType::PPMB;
    else return ImageType::PLOT;
  }


  struct ImageHeader
  {
    ImageType::types type;
    unsigned int data_offset;
    unsigned int data_length;
    unsigned int rows;
    unsigned int cols;
    unsigned int channels;
    unsigned int pixel_depth; // in byte (char counter)
    float max_val;

    std::string print()
    {
      std::stringstream out;
      out << TypeInfo::getName(type) << " - " << cols<<"x"<<rows<<"x"<<channels << " - " << data_offset << " " << data_length;
      return out.str();
    }
  };

  std::ostream& operator<< (std::ostream& out, ImageHeader& header)
  {
    out << TypeInfo::getMagic(header.type) << "\n"
        << header.cols<<"\n"
        << header.rows<<"\n"
        << int(header.max_val)<<"\n";
    return out;
  }

  int readSize(const std::string& file_name, unsigned int offset, unsigned int& rows, unsigned int& cols)
  {
    if( file_name == "" || !boost::filesystem::exists(file_name) )
    {
      std::cerr << "[MLUtils] Could not find file " << file_name << "." << std::endl;
      return (-1);
    }

    std::ifstream fs;
    fs.open(file_name.c_str());
    if( !fs.is_open() || fs.fail() )
    {
      std::cerr << "[MLUtils] Could not open file " << file_name << "." << std::endl;
      return (-1);
    }
    fs.seekg(offset);
    std::string line;
    std::vector<std::string> st;

    rows = 0;

    try
    {
      getline(fs,line);
      boost::trim(line);
      boost::split(st, line, boost::is_any_of("\t\r "), boost::token_compress_on);
      cols = st.size();
      ++rows;

      while( getline(fs, line) ) { if(line!="") ++rows; }
    }
    catch (const char* exception)
    {
      std::cerr << "[MLUtils::read] " << exception << std::endl;
      fs.close();
      return (-1);
    }

    fs.close();

    return 0;
  }

  int read(const std::string& file_name, Eigen::MatrixXd& mat)
  {
    unsigned int rows, cols;
    readSize(file_name, 0, rows, cols);

    if( file_name == "" || !boost::filesystem::exists(file_name) )
    {
      std::cerr << "[MLUtils::read] Could not find file " << file_name << "." << std::endl;
      return (-1);
    }

    std::ifstream fs;
    fs.open(file_name.c_str());
    if( !fs.is_open() || fs.fail() )
    {
      std::cerr << "[MLUtils::read] Could not open file " << file_name << "." << std::endl;
      return (-1);
    }

    std::string line;
    std::vector<std::string> st;
    mat.resize(rows, cols);
    int idx = 0;
    try
    {
      while( fs.good() )
      {
        getline(fs, line);
        if(line=="") continue;
        boost::trim(line);
        boost::split(st, line, boost::is_any_of("\t\r "), boost::token_compress_on);
        for( int c=0; c<st.size(); ++c) { mat(idx,c) = atof(st[c].c_str()); }
        ++idx;
      }
    }
    catch (const char* exception)
    {
      std::cerr << "[MLUtils::read] " << exception << std::endl;
      fs.close();
      return (-1);
    }

    fs.close();
    std::cout << "[MLUtils::read] Loaded " << rows << " x " << cols << " Matrix." << std::endl;
    return 0;
  }

  std::string betaToString(const Eigen::VectorXd& beta)
  {
    std::stringstream ss;
    for(int i=0; i<beta.rows(); ++i) {ss << "x"<<i<<" = " << beta(i) << "\n"; }
    return ss.str();
  }

  int readImage(const std::string& file_name, ImageHeader& header, std::vector<float>& values)
  {
    if( file_name == "" || !boost::filesystem::exists(file_name) )
    {
      std::cerr << "[MLUtils] Could not find file " << file_name << "." << std::endl;
      return (-1);
    }

    std::ifstream fs;
    fs.open(file_name.c_str());
    if( !fs.is_open() || fs.fail() )
    {
      std::cerr << "[MLUtils] Could not open file " << file_name << "." << std::endl;
      return (-1);
    }

    std::vector<std::string> words;
    std::vector<std::string> st;
    std::string line;

    try
    {
      getline(fs,line);
      boost::trim(line);
      boost::split(st, line, boost::is_any_of("\t\r "), boost::token_compress_on);
      for(int i=0; i<st.size(); ++i)
      {
        if(st[i][0] == '#') break;
        words.push_back(st[i]);
      }
      header.type = stringToType(words[0]);
      while(words.size() < TypeInfo::headerSize(header.type))
      {
        getline(fs,line);
        boost::trim(line);
        boost::split(st, line, boost::is_any_of("\t\r "), boost::token_compress_on);
        for(int i=0; i<st.size(); ++i)
        {
          if(st[i][0] == '#') break;
          words.push_back(st[i]);
        }
      }
      // TODO: assign words to header by type
      header.cols = atoi(words[1].c_str());
      header.rows = atoi(words[2].c_str());
      header.pixel_depth = 1;
      header.max_val = atof(words[3].c_str());
      header.data_offset = fs.tellg();
      fs.seekg(0,fs.end);
      header.data_length = fs.tellg() - header.data_offset;
      if(TypeInfo::isBinary(header.type))
      {
        fs.seekg(header.data_offset);
        char* buffer = new char[header.data_length];
        fs.read(buffer,header.data_length);
        // TODO: add capability to combine multiple chars to one float (8,16,32 bit)
        values.clear();
        for(int i=0; i<header.data_length; ++i) { values.push_back(float(buffer[i])); }
        delete[] buffer;
      }
      else
      {
        std::cout << "ASCII is not yet supported." << std::endl;
      }

      if (!fs)
      {
        std::cout << "[MLUtils] Error reading " << file_name << ": only " << fs.gcount() << " could be read";
        fs.close();
        return(-1);
      }
    }
    catch (const char* exception)
    {
      std::cerr << "[MLUtils::read] " << exception << std::endl;
      fs.close();
      return (-1);
    }

    fs.close();
    return 0;
  }

  int readImage(const std::string& file_name, Eigen::MatrixXd& mat)
  {
    ImageHeader header;
    std::vector<float> values;
    if(readImage(file_name, header, values)) return(-1);

    std::cout << "Loaded " << header.print() << std::endl;
    mat.resize(header.rows,header.cols);
    float norm = 1.0f / float(header.max_val);
    for(int r=0; r<header.rows; ++r)
    {
      for(int c=0; c<header.cols; ++c)
      {
        mat(r,c) = values[r*header.cols+c] * norm;
      }
    }

    return 0;
  }

  int writeImage(const std::string& file_name, ImageHeader& header, char* buffer, size_t size)
  {
    std::ofstream fs;
    fs.open(file_name.c_str());
    if(!fs.is_open())
    {
      std::cerr << "[MLUtils::write] Could not create " << file_name << std::endl;
      return -1;
    }

    try
    {
      fs << header;
      fs.write(buffer, size);
    }
    catch (const char* exception)
    {
      std::cerr << "[MLUtils::write] " << exception << std::endl;
      fs.close();
      return (-1);
    }
    fs.close();
    return 0;
  }

  int writeImage(const std::string& file_name, Eigen::MatrixXd& mat)
  {
    ImageHeader h;
    h.type = ImageType::PGMB;
    h.rows = mat.rows();
    h.cols = mat.cols();
    h.max_val = 255;
    char* buffer = new char[h.rows*h.cols];

    for(int r=0; r<h.rows; ++r)
    {
      for(int c=0; c<h.cols; ++c)
      {
        buffer[r*h.cols+c] = mat(r,c) * h.max_val;
      }
    }

    return writeImage(file_name, h, buffer, h.rows*h.cols);
    delete[] buffer;
    return 0;
  }

  void getAllFilesInFolder(const std::string& folder, const std::string& extension, std::vector<std::string>& file_names)
  {
    namespace fs = boost::filesystem;

    fs::path dir(folder);

    if ( fs::exists(dir) && fs::is_directory(dir))
    {
      fs::directory_iterator end;
      for( fs::directory_iterator it(dir) ; it != end ; ++it)
      {
        if (fs::is_regular_file(it->status()) && it->path().extension() == extension)
        {
          file_names.push_back(it->path().string());
        }
      }
    }
  }


}
