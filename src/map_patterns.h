#ifndef MAP_PATTERNS_H
#define MAP_PATTERNS_H

#include <vector>
#include <string>
#include "patterns_generators.h"
#include "hash_containers.h"


void map_patterns(
  const std::vector<std::string>& strings,
  int cutoff,
  char pattern_type,
  str2int& str2idx,
  ints* strings_subset,
  str2ints& pat2str
);


#endif // MAP_PATTERNS_H