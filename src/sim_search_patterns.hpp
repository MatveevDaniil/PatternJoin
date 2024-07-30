#ifndef SIM_SEARCH_PATTERNS_HPP
#define SIM_SEARCH_PATTERNS_HPP

#include <vector>
#include <string>
#include "map_patterns.hpp"
#include "file_io.hpp"
#include "patterns_generators.hpp"
#include "trim_strings.hpp"


void sim_search_patterns(
  const std::vector<std::string>& strings,
  int cutoff,
  char metric,
  str2int& str2idx,
  int_pair_set& out,
  ints* strings_subset = nullptr,
  bool include_duplicates = true
);

int sim_search_patterns(
  std::string file_name,
  int cutoff,
  char metric
);


#endif // SIM_SEARCH_PATTERNS_HPP