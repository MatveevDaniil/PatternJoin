#include "map_patterns.h"

void map_patterns(
  const std::vector<std::string>& strings,
  int cutoff,
  char pattern_type,
  str2int& str2idx,
  ints* strings_subset,
  str2ints& pat2str
) {
  PatternFuncType PatternFunc = getPatternFunc(cutoff, pattern_type);

  if (strings_subset == nullptr)
    for (std::string str: strings)
      for (const auto& pattern: PatternFunc(str, nullptr)) 
        pat2str[pattern].push_back(str2idx[str]);
  else
    for (int str_idx: *strings_subset)
      for (const auto& pattern: PatternFunc(strings[str_idx], nullptr)) 
        pat2str[pattern].push_back(str_idx);
}