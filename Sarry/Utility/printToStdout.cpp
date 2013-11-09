#include "printToStdout.hpp"
#include <cstddef>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <io.h>
#include <stdio.h>
#include <stdexcept>
#include <vector>

void Sarry::printToStdout(const boost::filesystem::path& file)
{
#ifdef _WINDOWS
  if(_setmode(_fileno(stdout), _O_BINARY) == -1)
    throw std::runtime_error("Unable to write in binary mode.");
#endif

  if(boost::filesystem::is_directory(file))
    throw std::invalid_argument("Cannot print directory to screen.");

  std::size_t size = boost::filesystem::file_size(file);
  std::vector<char> memory(size);
  std::ifstream in(file.string().c_str(), std::ios::binary);
  in.read(&memory[0], size);
  fwrite(&memory[0], 1, size, stdout);
}

