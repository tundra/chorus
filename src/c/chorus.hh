//- Copyright 2015 the Neutrino authors (see AUTHORS).
//- Licensed under the Apache License, Version 2.0 (see LICENSE).

#ifndef _CHORUS_HH
#define _CHORUS_HH

#include "stdc.h"

#include "std/stdvector.hh"
#include <string>

namespace chorus {

class Main {
public:
  // Add the user's shell executables to the given vector in prioritized order,
  // such that the first one added is the one to use by default, the second one
  // the one to use if the first one doesn't work, and so on. The same shell
  // can be added multiple times. At least one shell is guaranteed to be added.
  static void list_shells(std::vector<std::string> *out);

  // Main entry-point.
  static int main(int argc, const char *argv[]);
};

} // namespace chorus

#endif // _CHORUS_HH
