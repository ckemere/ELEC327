#!/bin/bash

# for file in *; do mv "$file" `echo $file | tr ' ' '_'` ; done
for file in *; do mv "$file" `echo $file | sed -e 's/\([^_]*\)_.*\.\(.*\)/\1.\2/'`; done
