#ifndef TRIMP_STRINGS_HPP
#define TRIMP_STRINGS_HPP

#include <string>

enum class TrimDirection {
    Start,
    End,
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

#endif // TRIMP_STRINGS_HPP