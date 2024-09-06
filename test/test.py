import os
import shutil
import subprocess
from itertools import product
from Levenshtein import hamming
from Levenshtein import distance as levenshtein


def get_distance(dist_name: str) -> str:
  if dist_name == 'hamming':
    return hamming
  if dist_name == 'levenshtein':
    return levenshtein


def generate_uniform_input(
    alphabet: str, 
    max_len: int
) -> tuple:
  input_fname = f'./test_data/{alphabet}^{max_len}'
  input_seqs = []
  if os.path.exists(input_fname):
    os.remove(input_fname)
  input_file = open(input_fname, 'a')
  for length in range(1, max_len + 1):
    for seq in product(alphabet, repeat=length):
      seq = ''.join(seq)
      print(seq, file=input_file, end='\n')
      input_seqs.append(seq)
  input_file.close()
  return input_fname, input_seqs


def generate_output(
    input_fname: str, 
    input_seqs: list[str],
    dist_name: str,
    cutoff: int
):
  distance = get_distance(dist_name)
  output_fname = f'{input_fname}_{dist_name}_{cutoff}'
  if os.path.exists(output_fname):
    os.remove(output_fname)
  output_file = open(output_fname, 'a')
  output_pairs_set = set()
  for i, seq1 in enumerate(input_seqs):
    for seq2 in input_seqs[i:]:
      if distance(seq1, seq2) <= cutoff:
        if seq1 > seq2:
          output_pairs_set.add((seq2, seq1))
        else:
          output_pairs_set.add((seq1, seq2))
  for seq1, seq2 in output_pairs_set:
    print(seq1, seq2, file=output_file, end='\n', sep=' ')
  output_file.close()


def generate_uniform_tests(alphabet_and_length_list, edit_distances, cutoff_parameters):
  if not os.path.exists('./test_data'):
    os.mkdir('./test_data')
  else:
    shutil.rmtree('./test_data')
    os.mkdir('./test_data')
  for alphabet, max_len in alphabet_and_length_list:
    print('Generating test input')
    print(f'\tStrings alphabet: {{{", ".join(alphabet)}}}\n\tmaximal string length: {max_len}')
    input_fname, input_seqs = generate_uniform_input(alphabet, max_len)
    print('Generating test output')
    print(f'\tEdit distances: {edit_distances}\n\tCutoff parameters: {cutoff_parameters}')
    for dist_name in edit_distances:
      for cutoff in cutoff_parameters:
        generate_output(input_fname, input_seqs, dist_name, cutoff)
    print('Done.')
    

def get_distance_param(dist_name: str) -> str:
  if dist_name == 'hamming':
    return 'H'
  if dist_name == 'levenshtein':
    return 'L'
  

def get_method_shortcut(method: str) -> str:
  if method == 'pattern':
    return 'p'
  if method == 'semi_pattern':
    return 'sp'
  if method == 'partition_pattern':
    return 'pp'


def read_out(fname: str) -> set[tuple[str]]:
  out = set()
  with open(fname, 'r') as f:
    for line in f:
      seq1, seq2 = line.split()
      out.add((seq1, seq2))
      out.add((seq2, seq1))
  return out


def compare_outputs(
    fname1: str,
    fname2: str
):
  out1 = read_out(fname1)
  out2 = read_out(fname2)
  if out1 != out2:
    print(list(out1 - out2)[:10])
    print('=' * 100)
    print(list(out2 - out1)[:10])
    return False
  else:
    return True


def check_project(
    input_fname: str,
    output_fname: str, 
    distance: str,
    cutoff: int
):
  methods = ['pattern', 'semi_pattern', 'partition_pattern']
  for method in methods:
    print(f'\tChecking method: {method}')
    method_shortcut = get_method_shortcut(method)
    pattern_run_command = '../build/pattern_join_50 --file_name {} --cutoff {} --metric_type {} --method {} --include_duplicates false'
    dist_param = get_distance_param(distance)
    run_command = pattern_run_command.format(input_fname, cutoff, dist_param, method)
    stderr = subprocess.run(run_command, shell=True, text=True, capture_output=True).stderr
    out_ext = f'{method_shortcut}_{cutoff}_{dist_param}'
    pattern_out_fname = f'{input_fname}_{out_ext}'
    try:
      compare_result = compare_outputs(pattern_out_fname, output_fname)
    except:
      print(f'not found the output file: {pattern_out_fname}')
      compare_result = False
    if compare_result == False:
      print(f'Error!!!!!!!\n\
            input_fname={input_fname}\n\
            output_fname={output_fname}\n\
            distance={distance}\n\
            cutoff={cutoff}\n\
            method={method}\n\
            {run_command}')
      assert False
    print('\tDone.')

def main():
  alphabet_and_length_list = [('abc', 8), ('ab', 10)]
  edit_distances = ('hamming', 'levenshtein')
  cutoff_parameters = (1, 2)

  generate_uniform_tests(alphabet_and_length_list, edit_distances, cutoff_parameters)

  for alphabet, max_length in alphabet_and_length_list:
    for distance in ('hamming', 'levenshtein'):
      for cutoff in 1, 2:
        print(f'Testing project for alphabet: {alphabet}, max_length: {max_length}, distance: {distance}, cutoff: {cutoff}')
        input_fname = f'./test_data/{alphabet}^{max_length}'
        output_fname = f'{input_fname}_{distance}_{cutoff}'
        check_project(input_fname, output_fname, distance, cutoff)
        print('Done.')
  print('All tests passed.')

if __name__ == '__main__':
  main()