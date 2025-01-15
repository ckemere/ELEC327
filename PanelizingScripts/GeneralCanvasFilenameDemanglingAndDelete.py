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
  try:
    os.makedirs(newdir, exist_ok=True)
    shutil.move(os.path.join(working_dir, f['origfile']), os.path.join(newdir, f['newfile']))
  except:
    print(f)
    assert(False)

