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

#endif // DUPLICATES_SEARCH_HPP