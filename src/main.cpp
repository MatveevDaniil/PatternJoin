#include <fstream>
#include <string>
#include <vector>
#include "sim_search_patterns.h"
#include "sim_search_semi_patterns.h"
#include "sim_search_part_patterns.h"


int main(int argc, char* argv[]) {
  if (argc < 5)
    throw std::runtime_error("usage: main <file_name> <cutoff> <metric> <method>");
  std::string file_name = argv[1];
  int cutoff = std::stoi(argv[2]);
  char metric = argv[3][0];
  std::string method = argv[4];
  if (method == "pattern")
    return sim_search_patterns(file_name, cutoff, metric);
  else if (method == "semi_pattern")
    return sim_search_semi_patterns(file_name, cutoff, metric);
  else if (method == "partition_pattern")
    return sim_search_part_patterns(file_name, cutoff, metric);
  else
    throw std::runtime_error("Invalid similarity join method");
}