#include "file_io.hpp"

void readFile(
  const std::string& file_name,
  std::vector<std::string>& strings,
  str2int& str2idx,
  bool include_duplicates,
  str2ints& str2idxs
) {
  std::ifstream file(file_name);
  if (!file) {
    throw std::runtime_error("File does not exist");
  }
  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty() && line.back() == '\n')
        line.pop_back();
    if (line == "")
      throw std::runtime_error("Empty line spotted in the input file\n");
    strings.push_back(line);
  }

  str2idx.reserve(strings.size());
  for (int i = 0; i < strings.size(); i++)
    str2idx[strings[i]] = i;
  if (include_duplicates) {
    str2idxs.reserve(strings.size());
    for (int i = 0; i < strings.size(); i++)
      str2idxs[strings[i]].push_back(i);

  }
  file.close();
}

void writeFile(
  const std::string& file_name,
  const int_pair_set& out,
  const std::vector<std::string>& strings,
  str2ints& str2idxs, 
  bool include_duplicates
) {
  std::ofstream out_file;
  out_file.open(file_name);
  if (!include_duplicates)
    for (const auto& pair : out)
      out_file << strings[pair.first] << " " << strings[pair.second] << "\n";
  else {
    str_pair_set unique_out;
    for (const auto& pair : out) {
      std::string str1 = strings[pair.first], str2 = strings[pair.second];
      if (str1 < str2)
        unique_out.insert({str1, str2});
      else
        unique_out.insert({str2, str1});
    }
    for (auto pair : unique_out) {
      std::string str1 = pair.first, str2 = pair.second;
      for (auto str_idx1 : str2idxs[str1])
        for (auto str_idx2 : str2idxs[str2]) {
          out_file << str_idx1 << " " << str_idx2 << "\n";
          out_file << str_idx2 << " " << str_idx1 << "\n";
        }
      }
  }
  out_file.close();
}