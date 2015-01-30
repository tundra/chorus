//- Copyright 2015 the Neutrino authors (see AUTHORS).
//- Licensed under the Apache License, Version 2.0 (see LICENSE).

BEGIN_C_INCLUDES
#include "utils/log.h"
#include <sys/unistd.h>
#include <pwd.h>
#include <errno.h>
END_C_INCLUDES

void Main::list_shells(std::vector<std::string> *out) {
  // Highest priority is the value of $SHELL, if there is one.
  char *shell_env = getenv("SHELL");
  if (shell_env != NULL)
    out->push_back(shell_env);
  // Second priority is the shell set in the passwords file.
  struct passwd pwbuf;
  memset(&pwbuf, 0, sizeof(pwbuf));
  char scratch[1024];
  uid_t uid = getuid();
  passwd *pw = NULL;
  if (getpwuid_r(uid, &pwbuf, scratch, 1024, &pw) == 0)
    // The string constructor copies the value so it's safe to use even if
    // scratch gets cleared.
    out->push_back(pwbuf.pw_shell);
  // Finally, /bin/sh is always a last-resort fallback.
  out->push_back("/bin/sh");
}
