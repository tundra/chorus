//- Copyright 2015 the Neutrino authors (see AUTHORS).
//- Licensed under the Apache License, Version 2.0 (see LICENSE).

#include "stdc.h"
#include "test/asserts.hh"
#include "test/unittest.hh"
#include <stdarg.h>

#define kTempBufferSize 256

// Dump an error on test failure.
void fail(const char *file, int line, const char *fmt, ...) {
  char formatted[kTempBufferSize];
  va_list args;
  va_start(args, fmt);
  vsnprintf(formatted, kTempBufferSize, fmt, args);
  va_end(args);
  fprintf(stderr, "%s:%i: %s\n", file, line, formatted);
  abort();
}
