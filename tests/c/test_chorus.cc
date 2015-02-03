//- Copyright 2015 the Neutrino authors (see AUTHORS).
//- Licensed under the Apache License, Version 2.0 (see LICENSE).

#include "test/asserts.hh"
#include "test/unittest.hh"

#include "chorus.hh"

using namespace chorus;

TEST(chorus, shells) {
  std::vector<std::string> shells;
  Main::list_shells(&shells);
  // There should always be at least a fallback shell.
  ASSERT_TRUE(shells.size() > 0);
  for (size_t i = 0; i < shells.size(); i++)
    // This touches the contents of all the shells which, if there is an
    // ownership issue, valgrind might be able to catch.
    ASSERT_TRUE(strlen(shells[i].c_str()) > 0);
}

TEST(chorus, main) {
  const char *argv[1] = {"test_chorus"};
  ASSERT_EQ(0, Main::main(1, argv));
}
