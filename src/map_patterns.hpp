#ifndef MAP_PATTERNS_HPP
#define MAP_PATTERNS_HPP

#include <vector>
#include <string>
#include "patterns_generators.hpp"
#include "hash_containers.hpp"
#include "trim_strings.hpp"


template <TrimDirection trim_direction>
void map_patterns(
  const std::vector<std::string>& strings,
  int cutoff,
  char pattern_type,
  str2int& str2idx,
  ints* strings_subset,
  str2ints& pat2str,
  int trim_size = 0
) {
  PatternFuncType PatternFunc = getPatternFunc(cutoff, pattern_type);

  if (strings_subset == nullptr)
    for (std::string str: strings)
      for (const auto& pattern: PatternFunc(str, nullptr)) 
        pat2str[pattern].push_back(str2idx[str]);
  else {
    if (trim_direction == TrimDirection::No)
      for (int str_idx: *strings_subset)
        for (const auto& pattern: PatternFunc(strings[str_idx], nullptr)) 
          pat2str[pattern].push_back(str_idx);
    else
      for (int str_idx: *strings_subset)
        for (const auto& pattern: PatternFunc(trimString<trim_direction>(strings[str_idx], trim_size), nullptr)) 
          pat2str[pattern].push_back(str_idx);
  }
}

#endif // MAP_PATTERNS_HPP