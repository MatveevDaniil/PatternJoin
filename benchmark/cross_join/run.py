import argparse
import csv
import hashlib
import itertools
import json
from pathlib import Path
import random
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parent
VARIANTS = ['dual_a', 'dual_small', 'stream_a',
            'stream_small', 'stream_large']


def write(path, strings):
  path.write_text(''.join(s + '\n' for s in strings))


def run(variant, a, b, cutoff, metric, pairs='-'):
  result = subprocess.run(
    [str(ROOT / 'bench'), variant, str(a), str(b),
     str(cutoff), metric, str(pairs)],
    capture_output=True, text=True, timeout=300, check=True)
  return json.loads(result.stdout)


def distance(a, b, metric):
  if metric == 'H':
    return sum(x != y for x, y in zip(a, b)) + abs(len(a)-len(b))
  row = list(range(len(b) + 1))
  for i, x in enumerate(a):
    new = [i + 1]
    for j, y in enumerate(b):
      new.append(min(new[-1]+1, row[j+1]+1, row[j]+(x != y)))
    row = new
  return row[-1]


def validate():
  rng = random.Random(731)
  words = [''.join(s) for n in range(5)
           for s in itertools.product('ab', repeat=n)]
  cases = [([], []), ([], words), (words, []),
           (words, words), (['a']*8, ['a']*13),
           (['a_b', 'a b', 'abc'], ['ab', 'acb', 'a_b'])]
  for _ in range(30):
    a = rng.choices(words, k=rng.randrange(1, 25))
    b = rng.choices(words, k=rng.randrange(1, 25))
    cases.extend([(a, b), (b, a)])
  count = 0
  with tempfile.TemporaryDirectory() as temp:
    a_path, b_path, out = [Path(temp)/s for s in ('a','b','out')]
    for a, b in cases:
      write(a_path, a)
      write(b_path, b)
      for metric, cutoff in itertools.product('LH', [1, 2]):
        expected = {(i, j) for i, x in enumerate(a)
                    for j, y in enumerate(b)
                    if distance(x, y, metric) <= cutoff}
        for variant in VARIANTS:
          run(variant, a_path, b_path, cutoff, metric, out)
          actual = {tuple(map(int, line.split()))
                    for line in out.read_text().splitlines()}
          assert actual == expected, (variant, metric, cutoff)
          count += 1
  print(f'Passed {count} independent oracle comparisons', flush=True)


def benchmark(args):
  rng = random.Random(731)
  data_dir = ROOT / 'inputs'
  data_dir.mkdir(exist_ok=True)
  rows = []
  manifest = []
  for source in args.inputs:
    raw = source.read_bytes()
    strings = raw.decode().splitlines()
    if len(strings) < 50000 or not all(strings):
      raise ValueError('Need at least 50,000 nonempty lines')
    if len(strings) != len(set(strings)):
      raise ValueError('Benchmark input must contain unique strings')
    rng.shuffle(strings)
    size_a = min(5000, len(strings)//3)
    a = strings[:size_a]
    a_path, b_path = data_dir/'a', data_dir/'b'
    write(a_path, a)
    manifest.append(dict(path=str(source), rows=len(strings),
                         sha256=hashlib.sha256(raw).hexdigest()))
    (ROOT/'manifest.json').write_text(json.dumps(manifest, indent=2))
    cases = [(f'nested_{n}', strings[:n]) for n in
             sorted(set([500, size_a, 50000, len(strings)]))]
    cases.extend([
      ('disjoint', strings[size_a:2*size_a]),
      ('half_shared', a[:size_a//2] +
       strings[size_a:size_a+size_a//2])])
    for case, b in cases:
      write(b_path, b)
      for cutoff in (1, 2):
        expected = None
        for repeat in range(args.repeats):
          order = VARIANTS.copy()
          rng.shuffle(order)
          for variant in order:
            result = run(variant, a_path, b_path, cutoff, 'L')
            signature = result['pairs'], result['checksum']
            if expected is None:
              expected = signature
            assert signature == expected, (source, case, variant)
            rows.append(dict(dataset=source.name, case=case,
              n_a=len(a), n_b=len(b), cutoff=cutoff, metric='L',
              variant=variant, repeat=repeat, **result))
            with (ROOT/'results.csv').open('w') as file:
              writer = csv.DictWriter(file, fieldnames=rows[0],
                                      lineterminator='\n')
              writer.writeheader()
              writer.writerows(rows)
          print(source.name, case, cutoff, repeat,
                'all variants agree', flush=True)
  (ROOT/'manifest.json').write_text(json.dumps(manifest, indent=2))


if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('inputs', nargs='*', type=Path)
  parser.add_argument('--repeats', type=int, default=3)
  args = parser.parse_args()
  if args.repeats < 1:
    parser.error('--repeats must be positive')
  validate()
  if args.inputs:
    benchmark(args)
