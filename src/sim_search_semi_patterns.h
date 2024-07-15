#ifndef SIM_SEARCH_SEMI_PATTERNS_H
#define SIM_SEARCH_SEMI_PATTERNS_H

#include <vector>
#include <string>
#include "map_patterns.h"
#include "hash_containers.h"
#include "file_io.h"
#include "patterns_generators.h"
#include "bounded_edit_distance.h"


void sim_search_semi_patterns(
  const std::vector<std::string>& strings,
  int cutoff,
  char metric,
  str2int& str2idx,
  int_pair_set& out,
  ints* strings_subset = nullptr,
  bool include_duplicates = true
);

int sim_search_semi_patterns(
  std::string file_name,
  int cutoff,
  char metric
);


#endif // SIM_SEARCH_SEMI_PATTERNS_H