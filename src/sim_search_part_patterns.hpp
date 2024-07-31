#ifndef SIM_SEARCH_PART_PATTERNS_HPP
#define SIM_SEARCH_PART_PATTERNS_HPP

#include <vector>
#include <string>
#include "map_patterns.hpp"
#include "hash_containers.hpp"
#include "sim_search_semi_patterns.hpp"
#include "sim_search_patterns.hpp"
#include "file_io.hpp"
#include "bounded_edit_distance.hpp"
#include "trim_strings.hpp"

template <TrimDirection trim_direction>
inline void check_part(
  std::vector<std::string> &strings,
  int cutoff,
  char metric,
  str2int& str2idx,
  str2ints& part2strings,
  int_pair_set& out
) {
  distance_k_ptr distance_k = get_distance_k(metric);
  for (auto entry = part2strings.begin(); entry != part2strings.end(); entry++) {
    int part_len = entry->first.size();
    if (entry->second.size() == 1)
      out.insert({entry->second[0], entry->second[0]});
    else if (entry->second.size() < 1e+2) {
      ints *string_indeces = &(entry->second);
      std::vector<std::string> trimmed_strings(string_indeces->size());
      for (int i = 0; i < string_indeces->size(); i++)
        trimmed_strings[i] = trimString<trim_direction>(strings[string_indeces->at(i)], part_len);
      for (int i = 0; i < string_indeces->size(); i++) {
        std::string str1 = trimmed_strings[i];
        int str_idx1 = string_indeces->at(i);
        out.insert({str_idx1, str_idx1});
        for (int j = i + 1; j < string_indeces->size(); j++) {
          std::string str2 = trimmed_strings[j];
          int str_idx2 = string_indeces->at(j);
          if (distance_k(str1, str2, cutoff)) {
            if (str_idx1 > str_idx2)
              out.insert({str_idx2, str_idx1});
            else
              out.insert({str_idx1, str_idx2});
          }
        }
      }
    } else {
      sim_search_semi_patterns_impl<trim_direction>(
        strings, cutoff, metric, str2idx, out, &entry->second, false, entry->first.size());
    }
  }
}

int sim_search_part_patterns(
  std::string file_name,
  int cutoff,
  char metric,
  bool include_duplicates
);


#endif // SIM_SEARCH_PART_PATTERNS_HPP