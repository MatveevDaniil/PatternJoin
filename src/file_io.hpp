#ifndef FILE_IO_HPP
#define FILE_IO_HPP

#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include "hash_containers.hpp"

void readFile(
  const std::string& file_name,
  std::vector<std::string>& strings,
  str2int& str2idx,
  bool include_duplicates,
  str2ints& str2idxs
);

void writeFile(
  const std::string& file_name,
  const int_pair_set& out,
  const std::vector<std::string>& strings,
  str2ints& str2idxs, 
  bool include_duplicates
);

#endif // FILE_IO_HPP