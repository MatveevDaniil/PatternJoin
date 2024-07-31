#include <fstream>
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
  int cutoff;
  char metric;
  std::string method;
  bool include_duplicates;
};

Options parse_arguments(int argc, char* argv[]) {
  Options options;
  int opt;
  int option_index = 0;

  struct option long_options[] = {
    {"file_name", 1, 0, 'f'},
    {"cutoff", 1, 0, 'c'},
    {"metric_type", 1, 0, 't'},
    {"method", 1, 0, 'm'},
    {"include_duplicates", 1, 0, 'd'},
    {0, 0, 0, 0}
  };

  while ((opt = getopt_long(argc, argv, "f:c:t:m:d:", long_options, &option_index)) != -1) {
    switch (opt) {
      case 'f':
        options.file_name = optarg;
        break;
      case 'c':
        options.cutoff = std::stoi(optarg);
        break;
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

  return options;
}

int main(int argc, char* argv[]) {
  if (argc < 11)
    throw std::runtime_error(
      "arguments: --file_name <file_name> --cutoff <cutoff> --metric_type <metric> --method <method> --include_duplicates <true/false>");

  Options opt = parse_arguments(argc, argv);
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
  
}