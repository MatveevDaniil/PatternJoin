#ifndef TRIMP_STRINGS_HPP
#define TRIMP_STRINGS_HPP

#include <string>

enum class TrimDirection {
    Start,
    End,
    Mid,
    No
};

template <TrimDirection> std::string trimString(
  const std::string& str, int trim_size);

template <>
inline std::string trimString<TrimDirection::Start>(
  const std::string& str, int trim_size
) {
  return str.substr(trim_size);
}

template <>
inline std::string trimString<TrimDirection::End>(
  const std::string& str, int trim_size
) {
  return str.substr(0, str.size() - trim_size);
}

template <>
inline std::string trimString<TrimDirection::No>(
  const std::string& str, int trim_size
) {
  return str;
}

inline std::string trimMidLev(
  const std::string& str, const std::string& substr
) {
  int part_len = str.size() / 3;
  int residue = str.size() % 3;
  if (part_len == substr.size()) {
    if (residue == 0) {
      if (str.compare(part_len, part_len, substr) == 0)
        return str.substr(0, part_len) + str.substr(part_len * 2);
      else 
        return str.substr(0, part_len - 1) + str.substr(part_len * 2 - 1);
    } else if (residue == 1) {
      if (str.compare(part_len, part_len, substr) == 0)
        return str.substr(0, part_len) + str.substr(part_len * 2);
      else 
        return str.substr(0, part_len + 1) + str.substr(part_len * 2 + 1);
    } else {
      return str.substr(0, part_len + 1) + str.substr(part_len * 2 + 1);
    }
  } else {
    if (residue == 1) {
      return str.substr(0, part_len) + str.substr(part_len * 2 + 1);
    } else {
      if (str.compare(part_len + 1, part_len + 1, substr) == 0)
        return str.substr(0, part_len + 1) + str.substr(part_len * 2 + 2);
      else 
        return str.substr(0, part_len) + str.substr(part_len * 2 + 1);
    }
  }
}

inline std::string trimMidHam(
  const std::string& str, const std::string& substr
) {
  int part_len = str.size() / 3;
  int residue = str.size() % 3;

  if (part_len == substr.size()) {
    if (residue == 0)  {
      return str.substr(0, part_len) + str.substr(part_len * 2);
    } else if (residue == 1) {
      if (str.compare(part_len, part_len, substr) == 0)
        return str.substr(0, part_len) + str.substr(part_len * 2);
      else 
        return str.substr(0, part_len + 1) + str.substr(part_len * 2 + 1);
    } else {
      return str.substr(0, part_len + 1) + str.substr(part_len * 2 + 1);
    }
  } else {
    return str.substr(0, part_len + 1) + str.substr(part_len * 2 + 2);
  }
}

using MidTrimFunc = std::string(*)(const std::string& str, const std::string& substr);
inline MidTrimFunc getMidTrimFunc(char metric) {
  if (metric == 'L')
    return trimMidLev;
  else
    return trimMidHam;
}

#endif // TRIMP_STRINGS_HPP