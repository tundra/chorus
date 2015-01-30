//- Copyright 2015 the Neutrino authors (see AUTHORS).
//- Licensed under the Apache License, Version 2.0 (see LICENSE).

#include "stdc.h"

#include "chorus.hh"
#include "plankton-inl.hh"

BEGIN_C_INCLUDES
#include "utils/log.h"
END_C_INCLUDES

using namespace chorus;
using namespace plankton;

int Main::main(int argc, const char *argv[]) {
  std::vector<std::string> shells;
  list_shells(&shells);
  for (size_t i = 0; i < shells.size(); i++)
    HEST("%i %s", i, shells[i].c_str());
  /*
  if ((ptr = x_getenv("SHELL")) == NULL) {
        uid_t uid = getuid();
        struct passwd pw;

        if (x_getpwuid(uid, &pw)) {
            (void) x_getlogin(uid, &pw);
        }
        if (!OkPasswd(&pw)
            || *(ptr = pw.pw_shell) == 0) {
            // this is the same default that xterm uses
            ptr = x_strdup("/bin/sh");
        }
    }
    */
  return 0;
}

#ifdef IS_GCC
#  include "chorus-posix.cc"
#endif
