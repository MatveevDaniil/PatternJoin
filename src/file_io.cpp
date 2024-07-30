#include "file_io.hpp"

double readFile(
  const std::string& file_name,
  std::vector<std::string>& strings
) {
  double avg_str_len = 0;

  std::ifstream file(file_name);
  if (!file) {
    throw std::runtime_error("File does not exist");
  }
  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty() && line.back() == '\n') {
        line.pop_back();
    }
    if (line == "")
      throw std::runtime_error("Empty line spotted in the input file\n");
    strings.push_back(line);
    avg_str_len += line.size();
  }
  file.close();

  avg_str_len /=strings.size();    
  return avg_str_len;
}

void writeFile(
  const std::string& file_name,
  const int_pair_set& out,
  const std::vector<std::string>& strings
) {
  std::ofstream out_file;
  out_file.open(file_name);
  for (const auto& pair : out)
    out_file << strings[pair.first] << " " << strings[pair.second] << "\n";
  out_file.close();
}

int estimatePatToStrSpace(
  double avg_str_len,
  int strings_count,
  char metric,
  int cutoff
) {
  if (cutoff == 1 && metric == 'H') {
    return std::round(strings_count * (avg_str_len + 1));
  } else if (cutoff == 2 && metric == 'H') {
    return std::round(strings_count * (avg_str_len * (avg_str_len + 1) / 2 + 2));
  } else if (cutoff == 1 && metric == 'L') {
    return std::round(strings_count * (avg_str_len * 2 + 1));
  } else if (cutoff == 2 && metric == 'L') {
    return std::round(strings_count * (2 * avg_str_len * avg_str_len + avg_str_len + 1));
  } else {
    throw std::invalid_argument("Choose metric param from {L, H} and cutoff from {0, 1, 2}");
  }
}