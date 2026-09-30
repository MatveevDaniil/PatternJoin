#include "sim_search_semi_patterns.hpp"
#include <chrono>
#include <iostream>
#include <sys/resource.h>

using Strings = std::vector<std::string>;

Strings read(const char* path) {
  std::ifstream file(path);
  if (!file)
    throw std::runtime_error("Cannot open input");
  Strings strings;
  std::string line;
  while (std::getline(file, line))
    strings.push_back(line);
  if (file.bad())
    throw std::runtime_error("Cannot read input");
  return strings;
}

str2ints cloud(const Strings& strings, PatternFuncType generate) {
  str2ints index;
  Strings patterns;
  for (int i = 0; i < strings.size(); ++i) {
    patterns.clear();
    generate(strings[i], &patterns);
    for (const auto& pattern : patterns)
      index[pattern].push_back(i);
  }
  return index;
}

int main(int argc, char** argv) {
  if (argc != 7) {
    std::cerr << "bench variant A B cutoff metric pairs_file\n";
    return 1;
  }
  try {
    const std::string variant = argv[1];
    const auto a = read(argv[2]);
    const auto b = read(argv[3]);
    const int cutoff = std::stoi(argv[4]);
    const char metric = argv[5][0];
    auto generate = getPatternFunc(cutoff, 'S');
    auto distance = get_distance_k(metric);
    int_pair_set out;
    size_t keys_a = 0, keys_b = 0;
    auto start = std::chrono::steady_clock::now();
    auto built = start;
    auto accept = [&](int i, int j) {
      if (!out.count({i, j}) && distance(a[i], b[j], cutoff))
        out.insert({i, j});
    };
    if (variant == "dual_a" || variant == "dual_small") {
      auto ca = cloud(a, generate);
      auto cb = cloud(b, generate);
      keys_a = ca.size();
      keys_b = cb.size();
      built = std::chrono::steady_clock::now();
      const bool swap = variant == "dual_small" &&
        ca.size() > cb.size();
      const auto& probe = swap ? cb : ca;
      const auto& index = swap ? ca : cb;
      for (const auto& entry : probe) {
        auto found = index.find(entry.first);
        if (found == index.end())
          continue;
        for (int i : entry.second)
          for (int j : found->second)
            accept(swap ? j : i, swap ? i : j);
      }
    } else if (variant == "stream_a" ||
               variant == "stream_small" ||
               variant == "stream_large") {
      const bool swap =
        (variant == "stream_small" && a.size() > b.size()) ||
        (variant == "stream_large" && a.size() < b.size());
      const auto& indexed = swap ? b : a;
      const auto& probe = swap ? a : b;
      auto index = cloud(indexed, generate);
      (swap ? keys_b : keys_a) = index.size();
      built = std::chrono::steady_clock::now();
      Strings patterns;
      for (int j = 0; j < probe.size(); ++j) {
        patterns.clear();
        generate(probe[j], &patterns);
        for (const auto& pattern : patterns) {
          auto found = index.find(pattern);
          if (found == index.end())
            continue;
          for (int i : found->second)
            accept(swap ? j : i, swap ? i : j);
        }
      }
    } else {
      throw std::runtime_error("Unknown variant");
    }
    auto end = std::chrono::steady_clock::now();
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    double rss = usage.ru_maxrss;
#ifndef __APPLE__
    rss *= 1024;
#endif
    uint64_t checksum = 0;
    for (const auto& pair : out) {
      uint64_t value = (uint64_t(pair.first) << 32) |
        uint32_t(pair.second);
      value += 0x9e3779b97f4a7c15ULL;
      value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
      value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
      checksum += value ^ (value >> 31);
    }
    auto seconds = [](auto duration) {
      return std::chrono::duration<double>(duration).count();
    };
    std::cout << "{\"seconds\":" << seconds(end - start)
      << ",\"build_seconds\":" << seconds(built - start)
      << ",\"peak_mib\":" << rss / (1024 * 1024)
      << ",\"pairs\":" << out.size()
      << ",\"checksum\":\"" << checksum << "\""
      << ",\"keys_a\":" << keys_a
      << ",\"keys_b\":" << keys_b << "}\n";
    if (std::string(argv[6]) != "-") {
      std::ofstream file(argv[6]);
      file.exceptions(std::ios::failbit | std::ios::badbit);
      for (const auto& pair : out)
        file << pair.first << ' ' << pair.second << '\n';
      file.close();
    }
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
