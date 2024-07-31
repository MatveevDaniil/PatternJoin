#include "sim_search_semi_patterns.hpp"

int sim_search_semi_patterns(
  std::string file_name,
  int cutoff,
  char metric,
  bool include_duplicates
) {
  std::vector<std::string> strings;
  str2int str2idx;
  str2ints str2idxs;
  readFile(file_name, strings, str2idx, include_duplicates, str2idxs);

  int_pair_set out;
  sim_search_semi_patterns_impl<TrimDirection::No>(strings, cutoff, metric, str2idx, out, nullptr, true);
  std::string out_file_name = file_name + "_sp_" + std::to_string(cutoff) + "_" + metric;
  writeFile(out_file_name, out, strings, str2idxs, include_duplicates);
  return 0;
}