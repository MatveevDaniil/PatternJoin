#include "sim_search_semi_patterns.hpp"

int sim_search_semi_patterns(
  std::string file_name,
  int cutoff,
  char metric
) {
  std::vector<std::string> strings;
  str2ints pat2str;
  double avg_str_len = readFile(file_name, strings);
  pat2str.reserve(estimatePatToStrSpace(avg_str_len, strings.size(), metric, cutoff));
  str2int str2idx;
  str2idx.reserve(strings.size());
  for (int i = 0; i < strings.size(); i++)
    str2idx[strings[i]] = i;
  int_pair_set out;
  sim_search_semi_patterns_impl<TrimDirection::No>(strings, cutoff, metric, str2idx, out, nullptr, true);
  std::string out_file_name = file_name + "_sp_" + std::to_string(cutoff) + "_" + metric;
  writeFile(out_file_name, out, strings);
  return 0;
}