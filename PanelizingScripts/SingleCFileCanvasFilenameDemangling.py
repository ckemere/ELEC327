#!/usr/bin/python3

import glob
import os
import re
import sys
import shutil

if len(sys.argv) < 2:
  raise(ValueError('No arguments. Specify directory for parsing.'))
else:
  working_dir = os.path.abspath(sys.argv[1])
  if os.path.isdir(working_dir):
    print(working_dir)
  else:
    raise(ValueError('{} is not a directory'.format(working_dir)))


files = []
for (dirpath, dirnames, filenames) in os.walk(working_dir):
    files.extend(filenames)
    break

print(files)

parse = re.compile("(?P<name>[^\d_]*)_(LATE_)?\d*_\d*_(?P<filename>[^-]*)(-\d)?")

# messes up when there are accented characters:
# parse = re.compile("(?P<name>[a-zA-Z]*)_(LATE_)?\d*_\d*_(?P<filename>.*?)(-\d)?\.(?P<ext>.*)")

parsed_files = []
student_directories = []
student_files = {}
for f in files:
  try:
    split_fname = os.path.splitext(f)
    result = parse.match(split_fname[0]).groupdict()
    result['newfile'] = result['filename'] + split_fname[1] # add back extension
    result['origfile'] = f
    del(result['filename'])
    # del(result['ext'])
    parsed_files.append(result)
  except:
    print(f)
    assert(False)

for f in parsed_files:
  newdir = f['name']
  if not newdir in student_directories:
    student_directories.append(newdir)
    student_files[newdir] = []
    try:
        os.makedirs(newdir)
    except:
        print('Issue creating ', newdir)
        assert(False)

  try:
    shutil.move(os.path.join(working_dir, f['origfile']), os.path.join(newdir, f['newfile']))
    student_files[newdir].append(f['newfile'])
  except:
    print(f)
    assert(False)


def gen_compile_command(filename):
  compile_command = ["/Applications/ti/ccs1210/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/bin/cl430",
                     '-vmsp', '--use_hw_mpy=none', 
                     '--include_path=/Applications/ti/ccs1210/ccs/ccs_base/msp430/include',
                     '--include_path="."',
                     '--include_path=/Applications/ti/ccs1210/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/include',
                     '--define=__MSP430G2553__',
                     '-g',
                     '--diag_warning=225',
                     '--diag_wrap=off',
                     '--display_error_number',
                     '--preproc_with_compile',
                     '{}'.format(filename)]
  return compile_command

def gen_link_command(program, filenames):
  link_command = ["/Applications/ti/ccs1210/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/bin/cl430",
                     '-vmsp',
                     '--use_hw_mpy=none',
                     '--define=__MSP430G2553__',
                     '-g',
                     '--diag_warning=225',
                     '--diag_wrap=off',
                     '--display_error_number',
                     '-z',
                     '-m{}.map'.format(program),
                     '--heap_size=80',
                     '--stack_size=80',
                     '-i/Applications/ti/ccs1210/ccs/ccs_base/msp430/include',
                     '-i/Applications/ti/ccs1210/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/lib',
                     '-i/Applications/ti/ccs1210/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/include',
                     '--reread_libs',
                     '--diag_wrap=off',
                     '--display_error_number',
                     '--warn_sections',
                     '--xml_link_info={}_linkInfo.xml'.format(program),
                     '--use_hw_mpy=none',
                     '--rom_model',
                     '-o{}.out'.format(program),
                     ' '.join(filenames),
                     '../../lnk_msp430g2553.cmd',
                     '-llibc.a']
  return link_command


import subprocess
for dirname, dircontents in student_files.items():
  print(dirname)
  compile_command = gen_compile_command(dircontents[0])
  results = subprocess.run(compile_command, cwd=dirname, capture_output=True, text=True)
  if results.returncode != 0:
    print('Compile failure! ', dirname, '\n', results.stderr)
  else:
    if results.stderr:
      print(results.stderr)
    print(results.stdout)
    object_files = []
    for fn in dircontents:
      split_fname = os.path.splitext(fn)
      object_files.append(split_fname[0] + '.obj')
    link_command = gen_link_command('lab3', object_files)
    results = subprocess.run(link_command, cwd=dirname, capture_output=True, text=True)
    print(results.stdout)
    print(results.stderr)



