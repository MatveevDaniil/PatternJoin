# Cross-join strategy experiment

These implementations reuse the production semi-pattern generator,
hash containers, and distance checker. They do not change dispatch
or the production algorithm.

See [results](results.md), [raw runs](runs.csv), and the
[summary with ranges](summary.csv).

| Variant | Stored clouds | Probing |
| --- | --- | --- |
| `dual_a` | A and B | A's pattern keys against B |
| `dual_small` | A and B | Fewer distinct keys against more |
| `stream_a` | A | Generate B's patterns on demand |
| `stream_small` | Fewer input strings | Generate the other side |
| `stream_large` | More input strings | Generate the other side |

All variants preserve ordered A/B indices and duplicate occurrences.
They use the same verification and output deduplication policy.
"Smaller set" means fewer strings; "smaller cloud" means fewer keys.
Equal-sized inputs keep A indexed. No special A=B shortcut is used.

Build from the repository root:

```sh
c++ -std=c++20 -O2 -Isrc \
  benchmark/cross_join/bench.cpp \
  src/patterns_generators.cpp src/bounded_edit_distance.cpp \
  -o benchmark/cross_join/bench
python3 benchmark/cross_join/run.py /path/to/tcr /path/to/bcr
```

With no input files, the runner only performs oracle validation:
1,320 full pair-set comparisons covering both metrics and cutoffs,
empty sets and strings, repeated strings, and swapped inputs.

Benchmark inputs should have at least 50,000 unique, nonempty lines.
The runner shuffles each dataset with a fixed seed. A contains 5,000
strings; B is a nested prefix of 500, 5,000, 50,000, or all strings.
Two further cases contain 0% and 50% exact overlap at equal sizes.
No exact overlap does not imply no near matches. The equal nested
case is A=B. Both nonzero cutoffs use Levenshtein distance, matching
the original performance comparisons. Cutoff zero is a separate
exact-match algorithm and is outside this experiment.

Each measurement runs in a fresh process. Three repetitions use
shuffled variant order. Timing includes cloud construction, probing,
pair verification, output-set construction and cloud destruction;
it excludes input reading and output serialization. Build time is
also recorded separately. Peak RSS includes loaded strings, clouds,
and output pairs. It is process peak memory, not cloud memory alone.

Every measurement checks pair count and an order-independent 64-bit
checksum against other variants. This is a large-input consistency
check, not a replacement for the small-input full oracle checks.

Results go to `results.csv`; input paths, counts and SHA-256 hashes
go to `manifest.json`. Raw input data is not committed. Each run has
a five-minute timeout; errors stop the experiment without treating
incomplete results as successful. Run on an otherwise idle machine
and compare medians; timings near one another need more repetitions.
