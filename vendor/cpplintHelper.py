#!/usr/bin/python

# The MIT License (MIT)
#
# Copyright (c) 2014 CantTouchDis <bauschp@informatik.uni-freiburg.de>
# Copyright (c) 2014 brio1009 <christoph1009@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import argparse
import os
import subprocess
import sys
from os import walk

# Define paths etc. Get them from the command line.
parser = argparse.ArgumentParser(description = 'Lint the given directory and subdirectories with cpplint.py.')
parser.add_argument('path',
                    metavar = 'PATH',
                    type = str,
                    help = 'The directory which should be linted.')
parser.add_argument('--root=',
                    dest = 'root',
                    type = str,
                    help = 'root directory for header guards.')
args = parser.parse_args()

# Assign the variables.
srcPath = args.path  #os.path.join('..', '..', 'src', 'RayTracerLib')
rootString = ''
if args.root:
  rootString = '--root=' + args.root #'src'

# Get all the files.
def getAllFiles():
  # We walk over the directory because we also want subfiles.
  f = []
  for (dirPath, dirNames, fileNames) in walk(srcPath):
    # We need to walk over every files to add the complete path.
    for fileName in fileNames:
      f.append(os.path.join(dirPath, fileName))
  return f
  
# Filter out cpp files.
def getAllSourceFiles():
  files = getAllFiles()
  extensions = ('c', 'cc', 'cpp', 'h', 'hpp')
  return [file for file in files if file.endswith(extensions)]

# Check the given file with the linter.
def lintFile(filesToCheck):
  try:
    print rootString
    subprocess.check_call([sys.executable, os.path.join('..', 'cpplint.py'), rootString] + filesToCheck)
  except subprocess.CalledProcessError as error:
    return False
  return True

# Main.
def main():
  # Get the source files.
  srcFiles = getAllSourceFiles()
  filesToCheck = [file for file in srcFiles if file]
  # Don't return an error code, or else the build process stops.
  lintFile(filesToCheck)

# Execute main.
if __name__ == '__main__':
  main()
