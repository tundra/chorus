//- Copyright 2015 the Neutrino authors (see AUTHORS).
//- Licensed under the Apache License, Version 2.0 (see LICENSE).

BEGIN_C_INCLUDES
#include "utils/log.h"
#include <sys/unistd.h>
#include <pwd.h>
#include <errno.h>
END_C_INCLUDES

static passwd *buf_ptr = NULL;

// This should match the shell determination logic from xterm.
void Main::list_shells(std::vector<std::string> *out) {
  // Highest priority is the value of $SHELL, if there is one.
  char *shell_env = getenv("SHELL");
  if (shell_env != NULL)
    out->push_back(shell_env);
  // Second priority is the shell set in the passwords file.
  struct passwd pwbuf;
  memset(&pwbuf, 0, sizeof(pwbuf));
  size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  char *scratch = new char[bufsize];
  uid_t uid = getuid();
  passwd *pw = NULL;
  if (getpwuid_r(uid, &pwbuf, scratch, bufsize, &pw) == 0) {
    // The string constructor copies the value so it's safe to use even if
    // scratch gets cleared.
    out->push_back(pw->pw_shell);
    buf_ptr = pw;
  }
  delete[] scratch;
  // Finally, /bin/sh is always a last-resort fallback.
  out->push_back("/bin/sh");
}
