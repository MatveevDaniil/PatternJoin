#include "sim_search_patterns.h"

void sim_search_patterns(
  const std::vector<std::string>& strings,
  int cutoff,
  char metric,
  str2int& str2idx,
  int_pair_set& out,
  ints* strings_subset,
  bool include_duplicates
) {
  str2ints pat2str;
  map_patterns(strings, cutoff, metric, str2idx, strings_subset, pat2str);

  for (auto entry = pat2str.begin(); entry != pat2str.end(); entry++) {
    if (entry->second.size() > 1) {
      for (auto str_idx1 = entry->second.begin(); str_idx1 != entry->second.end(); ++str_idx1) {
        for (auto str_idx2 = str_idx1 + 1; str_idx2 != entry->second.end(); ++str_idx2) {
          if (*str_idx1 > *str_idx2) {
            out.insert({*str_idx2, *str_idx1});
          } else {  
            out.insert({*str_idx1, *str_idx2});
          }
        }
      }
    }
  }

  if (include_duplicates)
    for (int i = 0; i < strings.size(); i++)
      out.insert({i, i});
}


int sim_search_patterns(
  std::string file_name,
  int cutoff,
  char metric
) {
  std::vector<std::string> strings;
  double avg_str_len = readFile(file_name, strings);
  str2int str2idx;
  str2idx.reserve(strings.size());
  for (int i = 0; i < strings.size(); i++)
    str2idx[strings[i]] = i;
  int_pair_set out;

  sim_search_patterns(strings, cutoff, metric, str2idx, out, nullptr, true);
  std::string out_file_name = file_name + "_p_" + std::to_string(cutoff) + "_" + metric;
  writeFile(out_file_name, out, strings);
  return 0;
}