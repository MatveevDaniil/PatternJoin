#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include "getopt.h"
#include "duplicates_search.hpp"
#include "sim_search_patterns.hpp"
#include "sim_search_semi_patterns.hpp"
#include "sim_search_part_patterns.hpp"

enum class Method { Pattern, SemiPattern, PartitionPattern, Unknown };

struct Options {
  std::string file_name;
  std::vector<std::string> file_names;
  int cutoff;
  char metric;
  Method method = Method::Unknown;
  bool include_duplicates;
};

Options parse_arguments(int argc, char* argv[]) {
  Options options;
  int opt;
  int option_index = 0;
  bool input_selected = false;

  struct option long_options[] = {
    {"file_names", 1, 0, 'F'},
    {"file_name", 1, 0, 'f'},
    {"cutoff", 1, 0, 'c'},
    {"metric_type", 1, 0, 't'},
    {"method", 1, 0, 'm'},
    {"include_duplicates", 1, 0, 'd'},
    {0, 0, 0, 0}
  };

  while ((opt = getopt_long(argc, argv, "f:c:t:m:d:", long_options,
            &option_index)) != -1) {
    switch (opt) {
      case 'f':
      case 'F': {
        if (input_selected)
          throw std::runtime_error(
            "Use either --file_name or --file_names, once");

        const bool invalid_path =
          optarg[0] == '\0' || optarg[0] == '-';
        if (invalid_path)
          throw std::runtime_error("Expected an input file path");

        if (opt == 'f') {
          options.file_name = optarg;
          input_selected = true;
          break;
        }

        const bool has_second_file = optind < argc &&
          argv[optind][0] != '\0' && argv[optind][0] != '-';
        const bool has_third_file =
          optind + 1 < argc && argv[optind + 1][0] != '-';
        const bool has_exactly_two_files =
          has_second_file && !has_third_file;
        if (!has_exactly_two_files)
          throw std::runtime_error(
            "--file_names requires exactly two paths");

        options.file_names = {optarg, argv[optind]};
        optind++;
        input_selected = true;
        break;
      }
      case 'c':
        options.cutoff = std::stoi(optarg);
        break;
      case 't':
        if (std::string(optarg) == "H")
          options.metric = 'H';
        else if (std::string(optarg) == "L")
          options.metric = 'L';
        else
          throw std::runtime_error(
            "Invalid metric type, use `H` for hamming "
            "or `L` for levenshtein distance");
        break;
      case 'm':
        if (std::string(optarg) == "pattern")
          options.method = Method::Pattern;
        else if (std::string(optarg) == "semi_pattern")
          options.method = Method::SemiPattern;
        else if (std::string(optarg) == "partition_pattern")
          options.method = Method::PartitionPattern;
        else
          options.method = Method::Unknown;
        break;
      case 'd':
        if (std::string(optarg) == "true")
          options.include_duplicates = true;
        else if (std::string(optarg) == "false")
          options.include_duplicates = false;
        else
          throw std::runtime_error(
            "Invalid value for include_duplicates, "
            "use `true` or `false`");
        break;
      default:
        throw std::runtime_error("Unknown option");
    }
  }

  if (!input_selected)
    throw std::runtime_error("Specify --file_name or --file_names");
  if (optind != argc)
    throw std::runtime_error(
      "--file_name takes one path; --file_names takes two");
  return options;
}

int main(int argc, char* argv[]) {
  if (argc < 11)
    throw std::runtime_error(
      "arguments: (--file_name <file> | "
      "--file_names <file_a> <file_b>) "
      "--cutoff <cutoff> --metric_type <metric> "
      "--method <method> --include_duplicates <true/false>");

  Options opt = parse_arguments(argc, argv);
  const bool cross_join = opt.file_names.size() == 2;
  if (cross_join) {
    switch (opt.method) {
      case Method::SemiPattern:
        return sim_search_semi_patterns(
          opt.file_names[0], opt.file_names[1], opt.cutoff,
          opt.metric, opt.include_duplicates);
      case Method::Pattern:
      case Method::PartitionPattern:
        throw std::runtime_error(
          "Two-dataset joins are currently implemented "
          "only for semi_pattern");
      default:
        throw std::runtime_error(
          "Invalid similarity join method use `pattern`, "
          "`semi_pattern` or `partition_pattern`");
    }
  } else {
    if (opt.cutoff == 0) {
      duplicates_search(opt.file_name);
      return 0;
    }

    switch (opt.method) {
      case Method::Pattern:
        return sim_search_patterns(
          opt.file_name, opt.cutoff, opt.metric,
          opt.include_duplicates);
      case Method::SemiPattern:
        return sim_search_semi_patterns(
          opt.file_name, opt.cutoff, opt.metric,
          opt.include_duplicates);
      case Method::PartitionPattern:
        return sim_search_part_patterns(
          opt.file_name, opt.cutoff, opt.metric,
          opt.include_duplicates);
      default:
        throw std::runtime_error(
          "Invalid similarity join method use `pattern`, "
          "`semi_pattern` or `partition_pattern`");
    }
  }
}
