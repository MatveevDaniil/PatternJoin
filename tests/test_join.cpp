#include "join.hpp"
#include <iostream>
#include <random>

int distance(const std::string& a, const std::string& b, char metric) {
    if (metric == 'H') {
        int d = static_cast<int>(std::max(a.size(), b.size()) - std::min(a.size(), b.size()));
        for (std::size_t i = 0; i < std::min(a.size(), b.size()); ++i) d += a[i] != b[i];
        return d;
    }
    std::vector<int> row(b.size() + 1);
    std::iota(row.begin(), row.end(), 0);
    for (std::size_t i = 0; i < a.size(); ++i) {
        int diagonal = row[0];
        row[0] = static_cast<int>(i + 1);
        for (std::size_t j = 0; j < b.size(); ++j) {
            int old = row[j + 1];
            row[j + 1] = std::min({row[j] + 1, old + 1, diagonal + (a[i] != b[j])});
            diagonal = old;
        }
    }
    return row.back();
}
void check(const std::vector<std::string>& a, const std::vector<std::string>& b) {
    for (char metric : {'L', 'H'}) for (int k = 0; k <= 2; ++k) {
        patternjoin::Pairs expected;
        for (std::size_t i = 0; i < a.size(); ++i)
            for (std::size_t j = 0; j < b.size(); ++j)
                if (distance(a[i], b[j], metric) <= k) expected.emplace_back(i, j);
        if (patternjoin::join(a, b, k, metric) != expected)
            throw std::runtime_error("Cross join differs from exhaustive oracle");
        if (&a == &b && patternjoin::join(a, k, metric) != expected)
            throw std::runtime_error("Self join differs from oracle");
    }
}
int main() {
    std::vector<std::string> words{""}, level{""};
    for (int n = 1; n <= 5; ++n) {
        std::vector<std::string> next;
        for (const auto& s : level) for (char c : {'a', 'b'}) next.push_back(s + c);
        words.insert(words.end(), next.begin(), next.end());
        level = next;
    }
    words.push_back("a"); words.push_back("*"); words.push_back(std::string("a\0b", 3));
    check(words, words); check({}, words); check(words, {});
    check({"cat", "cat", ""}, {"bat", "cat", "cat", "", ""});
    std::mt19937 rng(2026);
    for (int trial = 0; trial < 100; ++trial) {
        std::vector<std::string> a, b;
        for (auto* side : {&a, &b}) for (int n = rng() % 25; n > 0; --n) {
            std::string word;
            for (int l = rng() % 16; l > 0; --l) word += "abc*"[rng() % 4];
            side->push_back(word);
        }
        check(a, b); check(b, a);
    }
    for (int k : {-1, 3}) {
        try { patternjoin::join({}, {}, k, 'L'); return 1; }
        catch (const std::invalid_argument&) {}
    }
    try { patternjoin::join({}, {}, 1, 'X'); return 1; }
    catch (const std::invalid_argument&) {}
    std::cout << "Exhaustive and randomized cross-join oracle checks passed\n";
}
