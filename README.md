# PatternJoin
This project is a tool for words edit similarity joins (a.k.a. all-pairs similarity search) under small ($< 3$) edit distance constraints. The software is specifically optimized for edit similarity joins of amino-acid sequences from TCR/BCR profiles, where the number of words is relatively large ($10^5-10^6$) and the average length of words is relatively small ($< 20$). 

## What is Edit Similarity Join?
[_Edit distance_](https://en.wikipedia.org/w/index.php?search=Edit+distance&title=Special:Search&ns0=1) between two words is the minimal number of _elementary operations_ needed to transform one word to another. In this project, we consider [_Levenshtein distance_](https://en.wikipedia.org/wiki/Levenshtein_distance) (allows _substitutions_ and _insertions_/_deletions_ of single letters) and [_Hamming distance_](https://en.wikipedia.org/wiki/Hamming_distance) (allows _substitutions_ of any letters and _insertions_/_deletions_ of letters in the end). 

_Edit Similarity Join_ is a specific problem when we have a set of words $X=\{w_1, w_2, \ldots, w_n\}$ and we want to find all pairs of 'similar words' $Y_c=\{(w_i, w_j): distance(w_i, w_j) < c\}$, where $c \in \mathbb{N}_0$ is called a _cutoff_ parameter.

### Paper
In progress..🧙

## Building
### Build with CMake
From the command line (Windows/macOS/Linux):

```shell
> git clone https://github.com/MatveevDaniil/PatternJoin.git
> cd PatternJoin
> mkdir build
> cd build
> cmake ..
> make
```

## Usage
Assuming you are in the build directory 

### macOS/Linux
```shell
> pattern_join --file_name <..> --cutoff <..> --metric_type <..> --method <..> --include_duplicates <..>
```

### Windows
```shell
> pattern_join.exe --file_name <..> --cutoff <..> --metric_type <..> --method <..> --include_duplicates <..>
```

### Arguments
- `<file_name>`: The path to the input file.
- `<cutoff>`: The edit distance cutoff (`0`, `1` or `2`). If `cutoff` = 0, then the value of `metric_type`, `method` and `include_duplicates` does not matter.
- `<metric_type>`: The edit distance metric (`L` for Levenshtein, `H` for Hamming).
- `<method>`: The core method of edit similarity join (`pattern`, `semi_pattern`, or `partition_pattern`). As default, we recommend using `partition_pattern` as the most memory-efficient while still fast method. For more details take a look to [the benchmark](#benchmarks) and [the paper](#paper).
- `<include_duplicates>`: Consider duplicates in input (`true` or `false`). If `false` the program will ignore duplicate strings in the input and output unique pairs of strings. If `true` program will treat duplicate strings in the input as a pair (index, string) and output pairs of indeces. 

### Input file format
List of words separated by `\n`: `<word_1>\n<word_2>\n...`.

### Output file format
If `include_duplicates = true`: Space separated pairs of words separated by `\n`: `<word_i> <word_j>\n...`.
If `include_duplicates = false`: Space separated pairs of indeces separated by `\n`: `<idx_i> <idx_j>\n...`.


## Benchmarks
In progress..🧙

## Third-party software
Currently, the project is [using](./thirdparty/) third-party software for hashmap, hashsets, and inlined vectors. 
Hashmaps and hashset are taken from the [ankerl](https://github.com/martinus/unordered_dense) project and inlined vector from [gch](https://github.com/gharveymn/small_vector) project.
You can easily replace any of these containers with any other implementations of hashmap/hashset/vector by changing the corresponding line in [src/hash_containers.h](./src/hash_containers.hpp#L7-L11):
```c++
// src/hash_containers.h
using str2int = your_map<std::string, int>;
using ints = your_vector<int>;
using str2ints = your_map<std::string, ints>;
using int_pair_set = your_set<std::pair<int, int>>;
using str_pair_set = your_set<std::pair<std::string, std::string>>
```

## Roadmap
1. Benchmarks.
2. Finish documentation.
3. Create R/Python packages.
4. PAPER
5. Cover edit distances $\geq 3$.