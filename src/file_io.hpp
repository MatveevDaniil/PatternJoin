#ifndef FILE_IO_HPP
#define FILE_IO_HPP

#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include "hash_containers.hpp"

double readFile(
  const std::string& file_name,
  std::vector<std::string>& strings
);

int estimatePatToStrSpace(
  double avg_str_len,
  int strings_size,
  char metric,
  int cutoff
);

void writeFile(
  const std::string& file_name,
  const int_pair_set& out,
  const std::vector<std::string>& strings
);

#endif // FILE_IO_HPP