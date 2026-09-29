#include <fstream>
#include <filesystem>
#include <iostream>
#include <set>
#include "join.hpp"
#include <string>
#include <vector>
#include <stdexcept>
#include "getopt.h"
#include "duplicates_search.hpp"
#include "sim_search_patterns.hpp"
#include "sim_search_semi_patterns.hpp"
#include "sim_search_part_patterns.hpp"

struct Options {
  std::string file_name;
  std::string file_name_b;
  std::string output;
  bool cross = false;
  int cutoff = 1;
  char metric = 'L';
  std::string method = "partition_pattern";
  bool include_duplicates = true;
};

Options parse_arguments(int argc, char* argv[]) {
  Options options;
  int opt;
  int option_index = 0;

  struct option long_options[] = {
    {"file_name_b", 1, 0, 'b'},
    {"output", 1, 0, 'o'},
    {"file_name", 1, 0, 'f'},
    {"cutoff", 1, 0, 'c'},
    {"metric_type", 1, 0, 't'},
    {"method", 1, 0, 'm'},
    {"include_duplicates", 1, 0, 'd'},
    {0, 0, 0, 0}
  };

  while ((opt = getopt_long(argc, argv, "f:b:o:c:t:m:d:", long_options, &option_index)) != -1) {
    switch (opt) {
      case 'b':
        options.file_name_b = optarg;
        options.cross = true;
        break;
      case 'o':
        options.output = optarg;
        break;
      case 'f':
        options.file_name = optarg;
        break;
      case 'c': {
        std::size_t consumed = 0;
        options.cutoff = std::stoi(optarg, &consumed);
        if (consumed != std::string(optarg).size())
          throw std::runtime_error("cutoff must be an integer");
        break;
      }
      case 't':
        if (std::string(optarg) == "H")
          options.metric = 'H';
        else if (std::string(optarg) == "L")
          options.metric = 'L';
        else
          throw std::runtime_error("Invalid metric type, use `H` for hamming or `L` for levenshtein distance");
        break;
      case 'm':
        options.method = optarg;
        break;
      case 'd':
        if (std::string(optarg) == "true")
          options.include_duplicates = true;
        else if (std::string(optarg) == "false")
          options.include_duplicates = false;
        else
          throw std::runtime_error("Invalid value for include_duplicates, use `true` or `false`");
        break;
      default:
        throw std::runtime_error("Unknown option");
    }
  }

  if (options.file_name.empty() || (options.cross && options.file_name_b.empty()))
    throw std::runtime_error("Input file name is required");
  if (options.cutoff < 0 || options.cutoff > 2)
    throw std::runtime_error("cutoff must be 0, 1 or 2");
  if (options.cross && options.method != "cross_pattern")
    throw std::runtime_error("Two-set joins require --method cross_pattern");
  if (!options.cross && !options.output.empty())
    throw std::runtime_error("--output requires --file_name_b");
  return options;
}

std::vector<std::string> read_cross_input(const std::string& path) {
  std::ifstream input(path);
  if (!input) throw std::runtime_error("Cannot open input: " + path);
  std::vector<std::string> result;
  std::string line;
  while (std::getline(input, line)) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    result.push_back(line);
  }
  if (input.bad()) throw std::runtime_error("Cannot read input: " + path);
  return result;
}

int run(int argc, char* argv[]) {
  if (argc < 11)
    throw std::runtime_error(
      "arguments: --file_name <file_name> --cutoff <cutoff> --metric_type <metric> --method <method> --include_duplicates <true/false>");

  Options opt = parse_arguments(argc, argv);
  if (opt.cross) {
    auto a = read_cross_input(opt.file_name);
    auto b = read_cross_input(opt.file_name_b);
    auto pairs = patternjoin::join(a, b, opt.cutoff, opt.metric);
    const auto path = opt.output.empty() ? opt.file_name + "_cross_" +
      std::to_string(opt.cutoff) + "_" + opt.metric : opt.output;
    const auto resolved = std::filesystem::weakly_canonical(path);
    if (resolved == std::filesystem::weakly_canonical(opt.file_name) ||
        resolved == std::filesystem::weakly_canonical(opt.file_name_b) ||
        (std::filesystem::exists(path) &&
         (std::filesystem::equivalent(path, opt.file_name) ||
          std::filesystem::equivalent(path, opt.file_name_b))))
      throw std::runtime_error("Output must differ from input paths");
    std::ofstream output(path);
    if (!output) throw std::runtime_error("Cannot open output: " + path);
    std::set<std::pair<std::string, std::string>> written;
    for (const auto& pair : pairs) {
      if (opt.include_duplicates) output << pair.first << " " << pair.second << "\n";
      else if (written.emplace(a[pair.first], b[pair.second]).second)
        output << a[pair.first] << " " << b[pair.second] << "\n";
    }
    output.close();
    if (!output) throw std::runtime_error("Cannot write output: " + path);
    return 0;
  }
  if (opt.cutoff == 0) {
    duplicates_search(opt.file_name);
  } else {
    if (opt.method == "pattern")
      return sim_search_patterns(opt.file_name, opt.cutoff, opt.metric, opt.include_duplicates);
    else if (opt.method == "semi_pattern")
      return sim_search_semi_patterns(opt.file_name, opt.cutoff, opt.metric, opt.include_duplicates);
    else if (opt.method == "partition_pattern")
      return sim_search_part_patterns(opt.file_name, opt.cutoff, opt.metric, opt.include_duplicates);
    else
      throw std::runtime_error(
        "Invalid similarity join method use `pattern`, `semi_pattern` or `partition_pattern`");
  }
  return 0;
}

int main(int argc, char* argv[]) {
  try { return run(argc, argv); }
  catch (const std::exception& error) {
    std::cerr << "pattern_join: " << error.what() << '\n';
    return 1;
  }
}
