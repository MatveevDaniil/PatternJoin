#ifndef FILE_IO_H
#define FILE_IO_H

#include <vector>
#include <string>
#include <fstream>
#include "hash_containers.h"
#include <absl/container/flat_hash_set.h>

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

#endif // FILE_IO_H