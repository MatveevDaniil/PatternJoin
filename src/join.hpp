#ifndef PATTERNJOIN_JOIN_HPP
#define PATTERNJOIN_JOIN_HPP

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace patternjoin {
using Pair = std::pair<std::size_t, std::size_t>;
using Pairs = std::vector<Pair>;

namespace detail {
// No sentinel characters: signatures work with arbitrary byte strings, including NUL.
inline std::unordered_set<std::string> signatures(const std::string& word, int cutoff) {
    std::unordered_set<std::string> all{word}, frontier{word};
    for (int depth = 0; depth < cutoff; ++depth) {
        std::unordered_set<std::string> next;
        for (const auto& s : frontier)
            for (std::size_t p = 0; p < s.size(); ++p) {
                auto deleted = s.substr(0, p) + s.substr(p + 1);
                if (all.insert(deleted).second) next.insert(std::move(deleted));
            }
        frontier = std::move(next);
    }
    return all;
}

inline bool within(const std::string& a, const std::string& b, int k, char metric) {
    auto delta = a.size() > b.size() ? a.size() - b.size() : b.size() - a.size();
    if (delta > static_cast<std::size_t>(k)) return false;
    if (metric == 'H') {
        int distance = static_cast<int>(delta);
        for (std::size_t i = 0; i < std::min(a.size(), b.size()); ++i)
            if (a[i] != b[i] && ++distance > k) return false;
        return true;
    }
    // Banded Levenshtein DP; cells outside the band cannot lead to distance <= k.
    std::vector<int> previous(b.size() + 1, k + 1), current(b.size() + 1, k + 1);
    for (std::size_t j = 0; j <= std::min(b.size(), static_cast<std::size_t>(k)); ++j)
        previous[j] = static_cast<int>(j);
    for (std::size_t i = 1; i <= a.size(); ++i) {
        const auto lo = i > static_cast<std::size_t>(k) ? i - k : 1;
        const auto hi = std::min(b.size(), i + k);
        current[0] = i <= static_cast<std::size_t>(k) ? static_cast<int>(i) : k + 1;
        if (lo > 1) current[lo - 1] = k + 1;
        for (std::size_t j = lo; j <= hi; ++j)
            current[j] = std::min({previous[j] + 1, current[j - 1] + 1,
                                  previous[j - 1] + (a[i - 1] != b[j - 1])});
        if (hi < b.size()) current[hi + 1] = k + 1;
        previous.swap(current);
    }
    return previous[b.size()] <= k;
}

struct Group {
    std::string word;
    std::vector<std::size_t> indices;
};
inline std::vector<Group> group(const std::vector<std::string>& words) {
    std::unordered_map<std::string, std::size_t> ids;
    std::vector<Group> groups;
    for (std::size_t i = 0; i < words.size(); ++i) {
        auto result = ids.emplace(words[i], groups.size());
        if (result.second) groups.push_back({words[i], {}});
        groups[result.first->second].indices.push_back(i);
    }
    return groups;
}
} // namespace detail

// Ordered, zero-based original indices. Includes exact matches and all duplicate
// occurrences. Strings are byte sequences. Hamming includes trailing length difference.
// The callback API avoids materializing an output potentially as large as |A| * |B|.
template<class Emit>
void join_each(const std::vector<std::string>& a, const std::vector<std::string>& b,
               int cutoff, char metric, Emit emit) {
    if (cutoff < 0 || cutoff > 2) throw std::invalid_argument("cutoff must be 0, 1 or 2");
    if (metric != 'L' && metric != 'H') throw std::invalid_argument("metric must be L or H");
    if (a.empty() || b.empty()) return;
    auto left = detail::group(a), right = detail::group(b);
    // Index the smaller distinct set; restore A/B orientation before emission.
    const bool swapped = left.size() < right.size();
    const auto& indexed = swapped ? left : right;
    const auto& queries = swapped ? right : left;
    std::unordered_map<std::string, std::vector<std::size_t>> index;
    for (std::size_t i = 0; i < indexed.size(); ++i)
        for (const auto& signature : detail::signatures(indexed[i].word, cutoff))
            index[signature].push_back(i);
    std::vector<std::size_t> seen(indexed.size(), queries.size());
    for (std::size_t q = 0; q < queries.size(); ++q) {
        for (const auto& signature : detail::signatures(queries[q].word, cutoff)) {
            auto bucket = index.find(signature);
            if (bucket == index.end()) continue;
            for (auto candidate : bucket->second) {
                if (seen[candidate] == q) continue;
                seen[candidate] = q;
                if (!detail::within(queries[q].word, indexed[candidate].word, cutoff, metric)) continue;
                for (auto i : queries[q].indices)
                    for (auto j : indexed[candidate].indices)
                        if (swapped) emit(j, i); else emit(i, j);
            }
        }
    }
}

inline Pairs join(const std::vector<std::string>& a, const std::vector<std::string>& b,
                  int cutoff = 1, char metric = 'L') {
    Pairs result;
    join_each(a, b, cutoff, metric, [&](std::size_t i, std::size_t j) { result.emplace_back(i, j); });
    std::sort(result.begin(), result.end());
    return result;
}
inline Pairs join(const std::vector<std::string>& a, int cutoff = 1, char metric = 'L') {
    return join(a, a, cutoff, metric);
}
} // namespace patternjoin
#endif
