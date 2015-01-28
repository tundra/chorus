//- Copyright 2015 the Neutrino authors (see AUTHORS).
//- Licensed under the Apache License, Version 2.0 (see LICENSE).

#include "test/asserts.hh"
#include "test/unittest.hh"

#include "chorus.hh"

TEST(chorus, anything) {
  new int[3];
  ASSERT_EQ(4, chorus::Main::get_four());
}
