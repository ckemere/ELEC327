#!/bin/bash

# for file in *; do mv "$file" `echo $file | tr ' ' '_'` ; done
for file in *; do mv "$file" `echo $file | sed -e 's/\([^_]*\)_[0-9]*_[0-9]*_\([^.]*\)\.\(.*\)/\1_\2.\3/'`; done
