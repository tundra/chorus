#!/usr/bin/python
# Copyright 2015 the Neutrino authors (see AUTHORS).
# Licensed under the Apache License, Version 2.0 (see LICENSE).

import sys
import itertools

in_file = sys.argv[1]
out_file = sys.argv[2]

# Given a list, returns a list of blocks where each block contains blocksize
# elements from the input list, except possibly the last one if the input can
# not be evenly divided by blocksize.
def group_list(elms, blocksize):
  block = []
  for elm in elms:
    block.append(elm)
    if len(block) == blocksize:
      yield block
      block = []
  if len(block) > 0:
    yield block

FORMAT = """\
const size_t DATA_SIZE_NAME = %(len)i;
const byte_t DATA_NAME[%(len)i] = {
  %(data)s
};
"""

# Read the data and format the output.
output = None
with open(in_file, "rb") as file:
  bytes = [b for b in bytearray(file.read())]
  hexes = ["0x%02X" % b for b in bytes]
  groups = group_list(hexes, 13)
  contents = ",\n  ".join([", ".join(group) for group in groups])
  output = FORMAT % {
    "len": len(bytes),
    "data": contents
  }

# Write the output to the outfile.
with open(out_file, "wt") as file:
  file.write(output)
