#include "patterns_generators.hpp"

std::vector<std::string> Hamming1Patterns(
    const std::string& str,
    std::vector<std::string>* patterns
) {
  if (patterns == nullptr) {
    patterns = new std::vector<std::string>();
    patterns->reserve(str.length() + 1);
  }
  std::string pattern;
  for (int i = 0; i < static_cast<int>(str.length()); i++) {
    pattern = str;
    pattern[i] = '_';
    patterns->push_back(pattern);
  }
  pattern = str;
  pattern.push_back('_');
  patterns->push_back(pattern);
  return *patterns;
}

std::vector<std::string> Hamming2Patterns(
    const std::string& str,
    std::vector<std::string>* patterns
) {
  if (patterns == nullptr) {
    patterns = new std::vector<std::string>();
    int n = str.length();
    patterns->reserve(n * (n + 1) / 2 + n + 3);
  }
  std::string pattern;
  for (int i = 0; i < static_cast<int>(str.length()); i++) {
    for (int j = i + 1; j < static_cast<int>(str.length()); j++) {
      pattern = str;
      pattern[i] = pattern[j] = '_';
      patterns->push_back(pattern);
      pattern = str;
      pattern[i] = '_';
      pattern.push_back('_');
      patterns->push_back(pattern);
    }
  }
  pattern = str;
  pattern.push_back('_');
  pattern.push_back('_');
  patterns->push_back(pattern);
  pattern = str;
  pattern[static_cast<int>(str.length()) - 1] = '_';
  pattern.push_back('_');
  patterns->push_back(pattern);
  Hamming1Patterns(str, patterns);
  return *patterns;
}

std::vector<std::string> Levi1Patterns(
    const std::string& str,
    std::vector<std::string>* patterns
) {
   // Reserve space in the vector
  if (patterns == nullptr) {
    patterns = new std::vector<std::string>();
    patterns->reserve(str.length() * 2 + 1); 
  }
  std::string pattern, str1, str2;
  for (int i = 0; i < static_cast<int>(str.length()); i++) {
    pattern = str;
    pattern[i] = '_';
    // pattern.shrink_to_fit();
    patterns->push_back(pattern);

    pattern = str;
    pattern.insert(i, 1, '_');
    // pattern.shrink_to_fit();
    patterns->push_back(pattern);
  }
  pattern = str;
  pattern.push_back('_');
  patterns->push_back(pattern);
  // pattern.shrink_to_fit();
  return *patterns;
}

std::vector<std::string> Levi2Patterns(
    const std::string& str,
    std::vector<std::string>* patterns
) {
  if (patterns == nullptr) {
    patterns = new std::vector<std::string>();
    int n = str.length();
    patterns->reserve(2 * n * n + 3 * n + 2);
  }
  std::string pattern;
  for (int i = 0; i < static_cast<int>(str.length()); i++) {
    for (int j = 0; j < i; j++) {
      pattern = str;
      pattern.insert(j, 1, '_');
      pattern[i + 1] = '_';
      patterns->push_back(pattern); // k + 1
    }
    for (int j = i; j < static_cast<int>(str.size()); j++) {
      if (j > i) {
        pattern = str;
        pattern[i] = '_';
        pattern[j] = '_';
        patterns->push_back(pattern); // k
      }
      pattern = str;
      pattern[i] = '_';
      pattern.insert(j + 1, 1, '_');
      patterns->push_back(pattern); // k + 1
      pattern = str;
      pattern.insert(i, 1, '_');
      pattern.insert(j + 1, 1, '_');
      patterns->push_back(pattern); // k + 2
    }
    pattern = str;
    pattern.insert(i, 1, '_');
    pattern.push_back('_');
    patterns->push_back(pattern); // k + 2
  }
  pattern = str;
  pattern.push_back('_');
  pattern.push_back('_');
  patterns->push_back(pattern); // k + 2
  Levi1Patterns(str, patterns);
  return *patterns;
}

std::vector<std::string> semi1Patterns(
    const std::string& str,
    std::vector<std::string>* patterns
) {
  if (patterns == nullptr) {
    patterns = new std::vector<std::string>();
    patterns->reserve(str.length() + 1);
  }
  std::string pattern;
  for (int i = 0; i < static_cast<int>(str.length()); i++) {
    pattern = str;
    pattern.erase(i, 1);
    patterns->push_back(pattern);
  }
  patterns->push_back(str);
  return *patterns;
}

std::vector<std::string> semi2Patterns(
    const std::string& str,
    std::vector<std::string>* patterns
) {
  if (patterns == nullptr) {
    patterns = new std::vector<std::string>();
    patterns->reserve((str.length()) * (str.length() + 3) / 2);
  }
  std::string pattern;
  for (int i = 0; i < static_cast<int>(str.size()); ++i) {
    pattern = str;
    pattern.erase(i, 1);
    patterns->push_back(pattern);
    for (int j = i + 1; j < static_cast<int>(str.size()); ++j) {
      pattern = str;
      pattern.erase(i, 1);
      pattern.erase(j - 1, 1);
      patterns->push_back(pattern);
    }
  }
  patterns->push_back(str);
  return *patterns;
}

PatternFuncType getPatternFunc(int cutoff, char pattern_type) {
  PatternFuncType PatternFunc;
  if (cutoff == 1 && pattern_type == 'H')
    PatternFunc = Hamming1Patterns;
  else if (cutoff == 2 && pattern_type == 'H')
    PatternFunc = Hamming2Patterns;
  else if (cutoff == 1 && pattern_type == 'L')
    PatternFunc = Levi1Patterns;
  else if (cutoff == 2 && pattern_type == 'L')
    PatternFunc = Levi2Patterns;
  else if (cutoff == 1 && pattern_type == 'S')
    PatternFunc = semi1Patterns;
  else if (cutoff == 2 && pattern_type == 'S')
    PatternFunc = semi2Patterns;
  else
    throw std::invalid_argument("Choose pattern 'pattern_type' from {L, H, S} and 'cutoff' from {0, 1, 2}");
  return PatternFunc;
}