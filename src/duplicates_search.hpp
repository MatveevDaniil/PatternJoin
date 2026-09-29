#ifndef DUPLICATES_SEARCH_HPP
#define DUPLICATES_SEARCH_HPP

#include "file_io.hpp"
#include "hash_containers.hpp"

void duplicates_search(
  std::string file_name
) {
  std::vector<std::string> strings;
  str2int str2idx;
  str2ints str2idxs;
  readFile(file_name, strings, str2idx, true, str2idxs);
  std::string out_file_name = file_name + "_dupl";
  std::ofstream out_file;
  out_file.open(out_file_name);
  for (const auto& entry : str2idxs) {
    ints idxs = entry.second;
    for (int i = 0; i < idxs.size(); i++) {
      out_file << idxs[i] << " " << idxs[i] << "\n";
      for (int j = i + 1; j < idxs.size(); j++) {
        out_file << idxs[i] << " " << idxs[j] << "\n";
        out_file << idxs[j] << " " << idxs[i] << "\n";
      }
    }
  }
  out_file.close();
}

void duplicates_search(
  const std::vector<std::string>& strings_a,
  const std::vector<std::string>& strings_b,
  int_pair_set& out
) {
  str2ints str2idxs;
  for (int i = 0; i < strings_a.size(); i++)
    str2idxs[strings_a[i]].push_back(i);

  for (int j = 0; j < strings_b.size(); j++) {
    auto entry = str2idxs.find(strings_b[j]);
    if (entry != str2idxs.end())
      for (int i : entry->second)
        out.insert({i, j});
  }
}

void duplicates_search(
  std::string file_name_a,
  std::string file_name_b
) {
  std::vector<std::string> strings_a, strings_b;
  str2int str2idx;
  str2ints str2idxs;
  readFile(file_name_a, strings_a, str2idx, false, str2idxs);
  str2idx.clear();
  readFile(file_name_b, strings_b, str2idx, false, str2idxs);

  int_pair_set out;
  duplicates_search(strings_a, strings_b, out);
  std::string out_file_name = file_name_a + "_cross_dupl";
  std::ofstream out_file(out_file_name);
  for (const auto& pair : out)
    out_file << pair.first << " " << pair.second << "\n";
  out_file.close();
}

#endif // DUPLICATES_SEARCH_HPP
