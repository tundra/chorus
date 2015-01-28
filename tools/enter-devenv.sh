#!/bin/sh
# Copyright 2014 the Neutrino authors (see AUTHORS).
# Licensed under the Apache License, Version 2.0 (see LICENSE).

## Sets up a build environment. This file is copied among all the repos that use
## mkmk, the authoritative version lives in mkmk/tools. Don't change this
## directly.
##
## Note that this file is meant to be sourced so we can't use set -e or exit,
## otherwise we'll exit the parent shell. So instead we use explicit returns
## at each command that can fail.

if ! which mkmk > /dev/null; then
  if [ ! -d devenv ]; then
    if ! which virtualenv > /dev/null; then
      if ! which pip > /dev/null; then
        echo "Trying to install pip."
        sudo apt-get install python-pip || return 1
      fi
      echo "Trying to install virtualenv."
      sudo pip install virtualenv || return 1
    fi
    echo "Trying to create devenv."
    virtualenv devenv || return 1
  fi
  if [ ! -d mkmk ]; then
    echo "Trying to check out mkmk from github."
    git clone https://github.com/tundra/mkmk.git mkmk || return 1
  fi
  echo "Trying to install python package."
  . devenv/bin/activate || return 1
  cd mkmk
  python setup.py develop || return 1
  cd ..
fi
