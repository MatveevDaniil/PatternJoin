#ifndef BOUNDED_EDIT_DISTANCE_HPP
#define BOUNDED_EDIT_DISTANCE_HPP

#include <string>
#include <algorithm>
#include <cstdint>
#include <vector>

bool edit_distance_k(
    std::string a, 
    std::string b, 
    int k
);

bool hamming_distance_k(
    std::string a, 
    std::string b, 
    int k
);

using distance_k_ptr = bool (*)(std::string, std::string, int);
distance_k_ptr get_distance_k(char metric);

#endif // BOUNDED_EDIT_DISTANCE_HPP