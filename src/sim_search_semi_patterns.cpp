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

void sim_search_semi_patterns(
  const std::vector<std::string>& strings_a,
  const std::vector<std::string>& strings_b,
  int cutoff,
  char metric,
  int_pair_set& out
) {
  distance_k_ptr distance_k = get_distance_k(metric);
  PatternFuncType PatternFunc = getPatternFunc(cutoff, 'S');
  const bool swap_inputs = strings_a.size() > strings_b.size();
  const auto& indexed_strings = swap_inputs ? strings_b : strings_a;
  const auto& query_strings = swap_inputs ? strings_a : strings_b;
  str2ints pat2str;
  std::vector<std::string> patterns;

  for (int i = 0; i < indexed_strings.size(); i++) {
    patterns.clear();
    PatternFunc(indexed_strings[i], &patterns);
    for (const auto& pattern : patterns)
      pat2str[pattern].push_back(i);
  }

  for (int j = 0; j < query_strings.size(); j++) {
    patterns.clear();
    PatternFunc(query_strings[j], &patterns);
    for (const auto& pattern : patterns) {
      auto entry = pat2str.find(pattern);
      if (entry == pat2str.end())
        continue;
      for (int i : entry->second) {
        const auto pair = swap_inputs ? std::make_pair(j, i)
                                      : std::make_pair(i, j);
        if (out.count(pair) == 0 &&
            distance_k(strings_a[pair.first],
              strings_b[pair.second], cutoff))
          out.insert(pair);
      }
    }
  }
}

int sim_search_semi_patterns(
  std::string file_name_a,
  std::string file_name_b,
  int cutoff,
  char metric,
  bool include_duplicates
) {
  std::vector<std::string> strings_a, strings_b;
  str2int str2idx;
  str2ints str2idxs;
  readFile(file_name_a, strings_a, str2idx, false, str2idxs);
  str2idx.clear();
  readFile(file_name_b, strings_b, str2idx, false, str2idxs);

  int_pair_set out;
  sim_search_semi_patterns(strings_a, strings_b, cutoff, metric, out);
  std::string out_file_name = file_name_a + "_cross_sp_" + std::to_string(cutoff) + "_" + metric;
  std::ofstream out_file(out_file_name);
  out_file.exceptions(std::ios::failbit | std::ios::badbit);
  str_pair_set unique_out;
  for (const auto& pair : out) {
    if (include_duplicates)
      out_file << pair.first << " " << pair.second << "\n";
    else if (unique_out.insert({strings_a[pair.first], strings_b[pair.second]}).second)
      out_file << strings_a[pair.first] << " " << strings_b[pair.second] << "\n";
  }
  out_file.close();
  return 0;
}
