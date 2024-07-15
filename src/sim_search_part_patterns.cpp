#include "sim_search_part_patterns.h"

void check_part(
  std::vector<std::string> &strings,
  int cutoff,
  char metric,
  str2int& str2idx,
  str2ints& part2strings,
  int_pair_set& out,
  int parts_edit_distance = 0
) {
  if (parts_edit_distance == 0) {
    for (auto entry = part2strings.begin(); entry != part2strings.end(); entry++)
      if (entry->second.size() < 1e+3) {
        for (auto str_idx1 = entry->second.begin(); str_idx1 != entry->second.end(); ++str_idx1) {
          out.insert({*str_idx1, *str_idx1});
          for (auto str_idx2 = str_idx1 + 1; str_idx2 != entry->second.end(); ++str_idx2)
            if (edit_distance_k(strings[*str_idx1], strings[*str_idx2], cutoff)) {
              if (*str_idx1 > *str_idx2)
                out.insert({*str_idx2, *str_idx1});
              else
                out.insert({*str_idx1, *str_idx2});
            }
        }
      } else {
        sim_search_semi_patterns(strings, cutoff, metric, str2idx, out, &entry->second, false);
      }
  } else if (parts_edit_distance == 1) {
    str2ints pat2parts;

    std::vector<std::string> parts;
    parts.reserve(part2strings.size());
    for (auto entry = part2strings.begin(); entry != part2strings.end(); entry++)
      parts.push_back(entry->first);

    str2int part2idx;
    part2idx.reserve(parts.size());
    for (int i = 0; i < parts.size(); i++)
      part2idx[parts[i]] = i;
    
    map_patterns(parts, parts_edit_distance, 'L', part2idx, nullptr, pat2parts);

    float pat2parts_avg = 0;
    for (auto entry = pat2parts.begin(); entry != pat2parts.end(); entry++)
      pat2parts_avg += entry->second.size();
    pat2parts_avg /= pat2parts.size();
 
    float part2strings_avg = 0;
    for (auto entry = part2strings.begin(); entry != part2strings.end(); entry++)
      part2strings_avg += entry->second.size();
    part2strings_avg /= part2strings.size();

    for (auto entry = pat2parts.begin(); entry != pat2parts.end(); entry++)
      for (auto part_idx1 = entry->second.begin(); part_idx1 != entry->second.end(); ++part_idx1)
        for (auto part_idx2 = part_idx1; part_idx2 != entry->second.end(); ++part_idx2) 
          for (auto str_idx1: part2strings[parts[*part_idx1]])
            for (auto str_idx2: part2strings[parts[*part_idx2]])
              if (edit_distance_k(strings[str_idx1], strings[str_idx2], cutoff)) {
                if (str_idx1 > str_idx2)
                  out.insert({str_idx2, str_idx1});
                else
                  out.insert({str_idx1, str_idx2});
              }
  } else {
    throw std::invalid_argument("Only parts_edit_distance = 0,1 is supported");
  }
}

void sim_search_2parts(
  std::vector<std::string> &strings,
  char metric,
  str2int& str2idx,
  int_pair_set& out,
  bool include_duplicates = true,
  int cutoff = 1
) {
  str2ints start2idxs, end2idxs;
  for (int i = 0; i < strings.size(); i++) {
    std::string str = strings[i];
    int half_len = str.size() / 2;
    start2idxs[str.substr(0, half_len)].push_back(i);
    end2idxs[str.substr(half_len)].push_back(i);
    if (str.size() % 2 == 1) {
      start2idxs[str.substr(0, half_len + 1)].push_back(i);
      end2idxs[str.substr(half_len + 1)].push_back(i);
    }
  }
  check_part(strings, cutoff, metric, str2idx, end2idxs, out, 0);
  check_part(strings, cutoff, metric, str2idx, start2idxs, out, 0);
  if (include_duplicates)
    for (int i = 0; i < strings.size(); i++)
      out.insert({i, i});
}

void sim_search_3parts(
  std::vector<std::string> &strings,
  char metric,
  str2int& str2idx,
  int_pair_set& out,
  bool include_duplicates = true,
  int cutoff = 1
) {
  str2ints start2idxs, mid2idxs, end2idxs;
  for (int i = 0; i < strings.size(); i++) {
    std::string str = strings[i];
    int part_len = str.size() / 3;
    int residue = str.size() % 3;
    if (residue == 0) {
      start2idxs[str.substr(0, part_len)].push_back(i);
      mid2idxs[str.substr(part_len, part_len)].push_back(i);
      end2idxs[str.substr(part_len * 2)].push_back(i);
      mid2idxs[str.substr(part_len + 1, part_len)].push_back(i);
    } else if (residue == 1) {
      start2idxs[str.substr(0, part_len)].push_back(i);
      mid2idxs[str.substr(part_len, part_len)].push_back(i);
      end2idxs[str.substr(part_len * 2)].push_back(i);
      start2idxs[str.substr(0, part_len + 1)].push_back(i);
      mid2idxs[str.substr(part_len + 1, part_len)].push_back(i);
      end2idxs[str.substr(part_len * 2 + 1)].push_back(i);
      mid2idxs[str.substr(part_len, part_len + 1)].push_back(i);
    } else if (residue == 2) {
      start2idxs[str.substr(0, part_len + 1)].push_back(i);
      mid2idxs[str.substr(part_len + 1, part_len)].push_back(i);
      end2idxs[str.substr(part_len * 2 + 1)].push_back(i);
      mid2idxs[str.substr(part_len + 1, part_len + 1)].push_back(i);
      end2idxs[str.substr(part_len * 2 + 2)].push_back(i);
      start2idxs[str.substr(0, part_len)].push_back(i);
      mid2idxs[str.substr(part_len, part_len + 1)].push_back(i);
    }
  }
  check_part(strings, cutoff, metric, str2idx, start2idxs, out, 0);
  check_part(strings, cutoff, metric, str2idx, mid2idxs, out, 0);
  check_part(strings, cutoff, metric, str2idx, end2idxs, out, 0);
  if (include_duplicates)
    for (int i = 0; i < strings.size(); i++)
      out.insert({i, i});
}

int sim_search_part_patterns(
  std::string file_name,
  int cutoff,
  char metric
) {
  std::vector<std::string> strings;
  readFile(file_name, strings);
  str2int str2idx;
  for (int i = 0; i < static_cast<int>(strings.size()); i++)
    str2idx[strings[i]] = i;
  int_pair_set out;

  if (cutoff == 1)
    sim_search_2parts(strings, metric, str2idx, out, true, cutoff);
  else if (cutoff == 2)
    sim_search_3parts(strings, metric, str2idx, out, true, cutoff);
  else
    throw std::invalid_argument("Cutoff=" + std::to_string(cutoff)  + " not implemented for this method.");

  std::string out_file_name = file_name + "_pp_" + std::to_string(cutoff) + "_" + metric;
  writeFile(out_file_name, out, strings);
  return 0;
}