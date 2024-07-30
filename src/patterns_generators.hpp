#ifndef PATTERNS_GENERATORS_HPP
#define PATTERNS_GENERATORS_HPP

#include <vector>
#include <string>
#include <stdexcept>


using PatternFuncType = std::vector<std::string>(*)(const std::string&, std::vector<std::string>*);
PatternFuncType getPatternFunc(int cutoff, char pattern_type);

std::vector<std::string> Hamming1Patterns(
    const std::string& str,
    std::vector<std::string>* patterns = nullptr
);

std::vector<std::string> Hamming2Patterns(
    const std::string& str,
    std::vector<std::string>* patterns = nullptr
);

std::vector<std::string> Levi1Patterns(
    const std::string& str,
    std::vector<std::string>* patterns = nullptr
);

std::vector<std::string> Levi2Patterns(
    const std::string& str,
    std::vector<std::string>* patterns = nullptr
);

std::vector<std::string> semi1Patterns(
    const std::string& str,
    std::vector<std::string>* patterns
);

std::vector<std::string> semi2Patterns(
    const std::string& str,
    std::vector<std::string>* patterns
);


#endif // PATTERNS_GENERATORS_HPP