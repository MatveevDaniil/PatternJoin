#include "sim_search_semi_patterns.hpp"
#include "duplicates_search.hpp"
#include <numeric>
#include <iostream>
#include <random>

int distance(
  const std::string& a, const std::string& b, char metric) {
  if (metric == 'H') {
    int d = static_cast<int>(
      std::max(a.size(), b.size()) - std::min(a.size(), b.size()));
    for (std::size_t i = 0; i < std::min(a.size(), b.size()); ++i)
      d += a[i] != b[i];
    return d;
  }
  std::vector<int> row(b.size() + 1);
  std::iota(row.begin(), row.end(), 0);
  for (std::size_t i = 0; i < a.size(); ++i) {
    int diagonal = row[0];
    row[0] = static_cast<int>(i + 1);
    for (std::size_t j = 0; j < b.size(); ++j) {
      int old = row[j + 1];
      row[j + 1] =
        std::min({row[j] + 1, old + 1, diagonal + (a[i] != b[j])});
      diagonal = old;
    }
  }
  return row.back();
}
void check(const std::vector<std::string>& a,
  const std::vector<std::string>& b) {
  for (char metric : {'L', 'H'})
    for (int k = 0; k <= 2; ++k) {
      int_pair_set expected;
      for (std::size_t i = 0; i < a.size(); ++i)
        for (std::size_t j = 0; j < b.size(); ++j)
          if (distance(a[i], b[j], metric) <= k)
            expected.insert({i, j});
      int_pair_set actual;
      if (k == 0)
        duplicates_search(a, b, actual);
      else
        sim_search_semi_patterns(a, b, k, metric, actual);
      if (actual != expected)
        throw std::runtime_error(
          "Cross join differs from exhaustive oracle");
    }
}
int main() {
  std::vector<std::string> words{""}, level{""};
  for (int n = 1; n <= 5; ++n) {
    std::vector<std::string> next;
    for (const auto& s : level)
      for (char c : {'a', 'b'})
        next.push_back(s + c);
    words.insert(words.end(), next.begin(), next.end());
    level = next;
  }
  words.push_back("a");
  words.push_back("*");
  words.push_back(std::string("a\0b", 3));
  check(words, words);
  check({}, words);
  check(words, {});
  check({}, {});
  check({"cat", "cat", ""}, {"bat", "cat", "cat", "", ""});
  check({"", "", ""}, {"", ""});
  check({"cat"}, {"dog", "fish"});
  check({"_", "*", "a b", "a\tb", "A", "a"},
    {"_", "**", "ab", "A", "a", ""});
  check({std::string("a\0b", 3), std::string("\0", 1)},
    {std::string("a\0c", 3), "ab", std::string("\0", 1)});
  check(std::vector<std::string>(20, "aaaa"),
    std::vector<std::string>(30, "aaaa"));

  std::string long_word;
  for (int i = 0; i < 80; i++)
    long_word += "abcd"[i % 4];
  std::vector<std::string> variants{long_word};
  for (int position : {0, 40, 79}) {
    auto changed = long_word;
    changed[position] = 'z';
    variants.push_back(changed);
    changed[20] = 'z';
    variants.push_back(changed);
    changed[60] = 'z';
    variants.push_back(changed);
    variants.push_back(
      long_word.substr(0, position) + long_word.substr(position + 1));
    variants.push_back(long_word.substr(0, position) + "z" +
      long_word.substr(position));
  }
  check({long_word}, variants);
  check(variants, {long_word});
  std::mt19937 rng(2026);
  for (int trial = 0; trial < 100; ++trial) {
    std::vector<std::string> a, b;
    for (auto* side : {&a, &b})
      for (int n = rng() % 25; n > 0; --n) {
        std::string word;
        for (int l = rng() % 16; l > 0; --l)
          word += "abc*"[rng() % 4];
        side->push_back(word);
      }
    check(a, b);
    check(b, a);
  }
  for (int trial = 0; trial < 100; trial++) {
    std::vector<std::string> a, b;
    for (int n = 0; n < 15; n++) {
      std::string word;
      for (int l = rng() % 20; l > 0; l--)
        word += "abc"[rng() % 3];
      a.push_back(word);
      b.push_back(word);
      b.push_back(word + "a");
      if (!word.empty())
        b.push_back(word.substr(1));
    }
    std::shuffle(b.begin(), b.end(), rng);
    check(a, b);
    check(b, a);
  }
  for (int k : {-1, 0, 3, 100}) {
    try {
      int_pair_set out;
      sim_search_semi_patterns({}, {}, k, 'L', out);
      return 1;
    } catch (const std::invalid_argument&) {
    }
  }
  try {
    int_pair_set out;
    sim_search_semi_patterns({}, {}, 1, 'X', out);
    return 1;
  } catch (const std::invalid_argument&) {
  }
  int_pair_set out{{99, 99}};
  duplicates_search({"cat", "cat"}, {"cat"}, out);
  if (out != int_pair_set{{99, 99}, {0, 0}, {1, 0}})
    return 1;
  out = {{99, 99}};
  sim_search_semi_patterns({"cat"}, {"bat"}, 1, 'L', out);
  if (out != int_pair_set{{99, 99}, {0, 0}})
    return 1;
  std::cout
    << "Exhaustive and randomized cross-join oracle checks passed\n";
}
