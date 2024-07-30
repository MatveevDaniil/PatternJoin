#ifndef SIM_SEARCH_PART_PATTERNS_H
#define SIM_SEARCH_PART_PATTERNS_H

#include <vector>
#include <string>
#include "map_patterns.h"
#include "hash_containers.h"
#include "sim_search_semi_patterns.h"
#include "sim_search_patterns.h"
#include "file_io.h"
#include "bounded_edit_distance.h"


int sim_search_part_patterns(
  std::string file_name,
  int cutoff,
  char metric
);


#endif // SIM_SEARCH_PART_PATTERNS_H