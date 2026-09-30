import itertools
import random
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


EXECUTABLE = Path(sys.argv.pop(1)).resolve()
METHODS = ['pattern', 'semi_pattern', 'partition_pattern']


def distance(a, b, metric):
  if metric == 'H':
    return abs(len(a) - len(b)) + sum(
      x != y for x, y in zip(a, b))
  row = list(range(len(b) + 1))
  for i, x in enumerate(a):
    next_row = [i + 1]
    for j, y in enumerate(b):
      next_row.append(min(
        next_row[-1] + 1, row[j + 1] + 1,
        row[j] + (x != y)))
    row = next_row
  return row[-1]


class CliTests(unittest.TestCase):
  invocations = 0

  @classmethod
  def tearDownClass(cls):
    print(f'Checked {cls.invocations} CLI invocations')

  def setUp(self):
    self.directory = tempfile.TemporaryDirectory()
    self.addCleanup(self.directory.cleanup)
    self.root = Path(self.directory.name)
    self.a = self.root / 'first input.txt'
    self.b = self.root / 'second input.txt'

  def write_inputs(self, a, b):
    for path, words in [(self.a, a), (self.b, b)]:
      path.write_bytes(''.join(s + '\n' for s in words).encode())

  def flags(self, cutoff=1, metric='L', method='semi_pattern',
            duplicates=True):
    return ['-c', str(cutoff), '-t', metric, '-m', method,
            '-d', str(duplicates).lower()]

  def run_cli(self, args, success=True):
    type(self).invocations += 1
    result = subprocess.run(
      [str(EXECUTABLE)] + list(map(str, args)),
      cwd=self.root, capture_output=True, timeout=15)
    message = f'{args!r}\n{result.stderr.decode(errors="replace")}'
    if success:
      self.assertEqual(result.returncode, 0, message)
    else:
      self.assertNotEqual(result.returncode, 0, message)
      self.assertTrue(result.stderr, message)
    return result

  def check_cross(self, a, b, cutoff, metric, duplicates,
                  method='semi_pattern'):
    self.write_inputs(a, b)
    self.run_cli(['--file_names', self.a, self.b] +
                 self.flags(cutoff, metric, method, duplicates))
    suffix = '_cross_dupl' if cutoff == 0 else (
      f'_cross_sp_{cutoff}_{metric}')
    output = Path(str(self.a) + suffix)
    rows = [tuple(line.split())
            for line in output.read_text().splitlines()]
    expected = {(i, j) for i, x in enumerate(a)
                for j, y in enumerate(b)
                if distance(x, y, metric) <= cutoff}
    if duplicates or cutoff == 0:
      rows = [tuple(map(int, row)) for row in rows]
    else:
      expected = {(a[i], b[j]) for i, j in expected}
    self.assertEqual(set(rows), expected)
    self.assertEqual(len(rows), len(expected))

  def test_cross_input_matrix(self):
    cases = [
      ([], []), ([], ['cat']), (['cat'], []),
      (['a'], ['a']), (['a'], ['b']),
      (['cat'], ['dog']),
      (['cat', 'cat'], ['cat', 'cat', 'cat']),
      (['cat', 'dog', 'cat'], ['dog', 'cat', 'bat']),
      (['a', 'aa', 'aaa'], ['b', 'ab', 'ba', 'aaaa']),
      (['abcd', 'abc', 'xabc'], ['ab', 'abxd', 'abcd']),
      (['_', '*', 'A', 'a'], ['_', '**', 'a', 'A']),
      (['x' * 80], ['x' * 79, 'x' * 78, 'x' * 77]),
    ]
    for a, b in cases:
      for cutoff, metric, duplicates in itertools.product(
          range(3), ['L', 'H'], [False, True]):
        with self.subTest(a=a, b=b, cutoff=cutoff,
                          metric=metric, duplicates=duplicates):
          self.check_cross(a, b, cutoff, metric, duplicates)

  def test_random_cross_joins(self):
    rng = random.Random(20260929)
    for trial in range(20):
      a = [''.join(rng.choices('abc', k=rng.randrange(1, 12)))
           for _ in range(rng.randrange(1, 15))]
      b = [s + 'a' for s in a] + a[:3] + ['zzzzzz']
      rng.shuffle(b)
      for left, right in [(a, b), (b, a)]:
        for cutoff, metric in itertools.product(range(3), ['L', 'H']):
          with self.subTest(trial=trial, cutoff=cutoff, metric=metric):
            self.check_cross(left, right, cutoff, metric, True)

  def test_zero_cutoff_ignores_method_and_output_mode(self):
    for method, metric, duplicates in itertools.product(
        METHODS + ['unused'], ['L', 'H'], [False, True]):
      with self.subTest(method=method, metric=metric,
                        duplicates=duplicates):
        self.check_cross(['cat', 'cat', 'dog'],
                         ['cat', 'dog', 'cat'], 0, metric,
                         duplicates, method)

  def test_self_join_regression(self):
    words = ['cat', 'bat', 'cats', 'dog', 'cat']
    for method, cutoff, metric, duplicates in itertools.product(
        METHODS, range(3), ['L', 'H'], [False, True]):
      with self.subTest(method=method, cutoff=cutoff,
                        metric=metric, duplicates=duplicates):
        self.write_inputs(words, [])
        self.run_cli(['--file_name', self.a] +
                     self.flags(cutoff, metric, method, duplicates))
        code = dict(zip(METHODS, ['p', 'sp', 'pp']))[method]
        suffix = '_dupl' if cutoff == 0 else f'_{code}_{cutoff}_{metric}'
        rows = [tuple(line.split()) for line in
                Path(str(self.a) + suffix).read_text().splitlines()]
        expected = {(i, j) for i, a in enumerate(words)
                    for j, b in enumerate(words)
                    if distance(a, b, metric) <= cutoff}
        if duplicates or cutoff == 0:
          actual = {tuple(map(int, row)) for row in rows}
        else:
          actual = {tuple(sorted(row)) for row in rows}
          expected = {tuple(sorted((words[i], words[j])))
                      for i, j in expected}
        self.assertEqual(actual, expected)

  def test_argument_layouts(self):
    self.write_inputs(['cat'], ['bat'])
    inputs = ['--file_names', self.a, self.b]
    options = [self.flags()[i:i + 2] for i in range(0, 8, 2)]
    for order in itertools.permutations([inputs] + options):
      args = [value for group in order for value in group]
      with self.subTest(args=args):
        self.run_cli(args)
    for inputs in [['--file_name=' + str(self.a)],
                   ['--file_names=' + str(self.a), self.b],
                   ['-f' + str(self.a)]]:
      self.run_cli(inputs + ['--cutoff=1', '--metric_type=L',
                            '--method=semi_pattern',
                            '--include_duplicates=true'])

  def test_invalid_inputs(self):
    self.write_inputs(['cat'], ['bat'])
    invalid = [
      [], ['--file_name'], ['--file_names'],
      ['--file_name', ''], ['--file_names', self.a],
      ['--file_names', '', self.b], ['--file_names', self.a, ''],
      ['--file_name', self.a, self.b],
      ['--file_names', self.a, self.b, 'extra'],
      ['--file_name', self.a, '--file_name', self.a],
      ['--file_names', self.a, self.b, '--file_name', self.a],
      ['--file_name', self.a, '--file_names', self.a, self.b],
      ['--file_names', self.a, self.b, '--file_names', self.a, self.b],
    ]
    for inputs in invalid:
      with self.subTest(inputs=inputs):
        self.run_cli(inputs + self.flags(), success=False)
    self.run_cli(['--file_names', self.a, self.b] +
                 self.flags() + ['extra'], success=False)

  def test_malformed_options(self):
    self.write_inputs(['cat'], ['bat'])
    inputs = ['--file_names', self.a, self.b]
    for flag, values in [
        ('-c', ['', 'abc', '1.5', '1x', '-1', '3', '9' * 40]),
        ('-t', ['', 'X', 'Levenshtein']),
        ('-d', ['', 'yes', '1', 'TRUE']),
        ('-m', ['', 'unknown', 'pattern', 'partition_pattern'])]:
      for value in values:
        with self.subTest(flag=flag, value=value):
          self.run_cli(inputs + self.flags() + [flag, value], False)
    for missing in ['-c', '-t', '-m', '-d']:
      flags = self.flags()
      position = flags.index(missing)
      del flags[position:position + 2]
      with self.subTest(missing=missing):
        extra = ['-t', 'L'] if missing == '-c' else ['-c', '1']
        self.run_cli(inputs + flags + extra, False)
    self.run_cli(inputs + self.flags() + ['--unknown'], False)
    for missing_value in ['--file_name', '--file_names', '-c',
                          '-t', '-m', '-d']:
      self.run_cli(inputs + self.flags() + [missing_value], False)

  def test_empty_files_and_blank_lines(self):
    for method, cutoff in itertools.product(METHODS, range(3)):
      self.write_inputs([], [])
      self.run_cli(['--file_name', self.a] +
                   self.flags(cutoff, method=method))
    for data, cutoff, side in itertools.product(
        [b'\n', b'cat\n\n', b'\ncat\n'], range(3), ['a', 'b']):
      self.write_inputs(['cat'], ['cat'])
      getattr(self, side).write_bytes(data)
      with self.subTest(data=data, cutoff=cutoff, side=side):
        self.run_cli(['--file_names', self.a, self.b] +
                     self.flags(cutoff), False)
    for data in [b'cat', b'cat\n']:
      self.a.write_bytes(data)
      self.b.write_bytes(data)
      self.run_cli(['--file_names', self.a, self.b] + self.flags())

  def test_missing_files_and_output_errors(self):
    for cutoff in range(3):
      self.write_inputs(['cat'], ['cat'])
      for inputs in [[self.a, self.root / 'missing'],
                     [self.root / 'missing', self.b],
                     [self.root, self.b], [self.a, self.root]]:
        self.run_cli(['--file_names'] + inputs +
                     self.flags(cutoff), False)
      suffix = '_cross_dupl' if cutoff == 0 else f'_cross_sp_{cutoff}_L'
      Path(str(self.a) + suffix).mkdir()
      self.run_cli(['--file_names', self.a, self.b] +
                   self.flags(cutoff), False)

  def test_self_join_output_errors(self):
    self.write_inputs(['cat'], [])
    for method, code in zip(METHODS, ['p', 'sp', 'pp']):
      for cutoff in range(3):
        suffix = '_dupl' if cutoff == 0 else f'_{code}_{cutoff}_L'
        Path(str(self.a) + suffix).mkdir(exist_ok=True)
        with self.subTest(method=method, cutoff=cutoff):
          self.run_cli(['--file_name', self.a] +
                       self.flags(cutoff, method=method), False)

  def test_same_file_and_replaced_output(self):
    words = ['cat', 'cat', 'bat']
    self.write_inputs(words, words)
    for cutoff in range(3):
      self.run_cli(['--file_names', self.a, self.a] +
                   self.flags(cutoff))
      suffix = '_cross_dupl' if cutoff == 0 else f'_cross_sp_{cutoff}_L'
      output = Path(str(self.a) + suffix)
      actual = {tuple(map(int, row.split()))
                for row in output.read_text().splitlines()}
      expected = {(i, j) for i, a in enumerate(words)
                  for j, b in enumerate(words)
                  if distance(a, b, 'L') <= cutoff}
      self.assertEqual(actual, expected)
      self.b.write_text('xxxxxxxx\n')
      self.run_cli(['--file_names', self.a, self.b] +
                   self.flags(cutoff))
      self.assertEqual(output.read_bytes(), b'')


if __name__ == '__main__':
  try:
    import resource
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
  except ImportError:
    pass
  unittest.main()
