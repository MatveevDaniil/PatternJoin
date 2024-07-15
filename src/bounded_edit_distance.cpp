#include "bounded_edit_distance.h"

bool edit_distance_k(
    std::string a, 
    std::string b, 
    int k
) {
    if (a == b)
        return true;

    if (a.size() > b.size())
        std::swap(a, b);
    
    if (b.size() - a.size() > k)
        return false;

    while (!a.empty() && a.back() == b.back()) {
        a.pop_back();
        b.pop_back();
    }

    int a_size = a.size(), start;
    for (start = 0; start < a_size && a[start] == b[start]; ++start);
    a = a.substr(start);
    b = b.substr(start);

    int b_size = b.size();
    a_size = a.size();

    if (a_size == 0)
        return true; // b_size <= k because otherwise b_size - a_size = b_size - 0 = b_size > k and we would have returned false earlier

    if (b_size <= k)
        return true;

    int size_d = b_size - a_size;

    int ZERO_K = std::min(k, a_size) / 2 + 2;
    auto array_size = size_d + ZERO_K * 2 + 2;

    std::vector<int> current_row(array_size, -1);
    std::vector<int> next_row(array_size, -1);

    int i = 0, kpp = k + 1;
    int condition_row = size_d + ZERO_K;
    int end_max = condition_row * 2;

    do {
        i++;

        std::swap(current_row, next_row);

        int start;
        int previous_cell;
        int current_cell = -1;
        int next_cell;

        if (i <= ZERO_K) {
            start = -i + 1;
            next_cell = i - 2;
        } else {
            start = i - ZERO_K * 2 + 1;
            next_cell = current_row[ZERO_K + start];
        }

        int end;
        if (i <= condition_row) {
            end = i;
            next_row[ZERO_K + i] = -1;
        } else {
            end = end_max - i;
        }

        for (int q = start, row_index = start + ZERO_K; q < end; q++, row_index++) {
            previous_cell = current_cell;
            current_cell = next_cell;
            next_cell = current_row[row_index + 1];

            int t = std::max(
                std::max(current_cell + 1, previous_cell),
                next_cell + 1
            );

            while (t < a_size && t + q < b_size && a[t] == b[t + q]) {
                t++;
            }

            next_row[row_index] = t;
        }
    } while (next_row[condition_row] < a_size && i <= kpp);

    return (i - 1) <= k;
}

bool hamming_distance_k(
    std::string a, 
    std::string b, 
    int k
) {
    if (a == b) 
        return true;
        
    int a_size = a.size(), b_size = b.size();
    int dist = abs(a_size - b_size);
    if (dist > k) 
        return false;

    int min_size = std::min(a_size, b_size);
    for (int i = 0; i < min_size; ++i) {
        if (a[i] != b[i]) { 
            dist++; 
            if (dist > k) 
                return false;
        }
    }

    return true;
}