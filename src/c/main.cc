//- Copyright 2015 the Neutrino authors (see AUTHORS).
//- Licensed under the Apache License, Version 2.0 (see LICENSE).

#include "c/stdc.h"
#include "chorus.hh"

extern "C" int main(int argc, const char *argv[]) {
  return chorus::Main::main(argc, argv);
}
