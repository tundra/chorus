//- Copyright 2015 the Neutrino authors (see AUTHORS).
//- Licensed under the Apache License, Version 2.0 (see LICENSE).

#include "chorus.hh"
#include "plankton-inl.hh"
#include "include/service.hh"

BEGIN_C_INCLUDES
#include "format.h"
#include "plugin.h"
#include "runtime.h"
#include "safe-inl.h"
#include "try-inl.h"
#include "utils/log.h"
END_C_INCLUDES

using namespace chorus;
using namespace plankton;

// Keeps track of the neutrino source code to run.
class Chorus {
public:
  static value_t load_library(neutrino::Runtime &runtime);
  static value_t run_main(neutrino::Runtime &runtime);

private:
  // The library data.
  static const size_t kNLibSize;
  static const byte_t kNLibData[];

  // The main program data.
  static const size_t kNMainSize;
  static const byte_t kNMainData[];
};

// The plugin that gets intalled into the runtime and supports interaction
// between the neutrino and C++ part of chorus.
class ChorusService : public neutrino::ForeignService {
public:
  // Installation hook.
  virtual neutrino::Maybe<> bind(neutrino::ForeignServiceBinder *config);

  // Yields a list of the shell executables.
  void list_shells(neutrino::ServiceRequest *request);
};

int Main::main(int argc, const char *argv[]) {
  value_t result = try_main(argc, argv);
  if (is_condition(result)) {
    ERROR("try_main(%i, *) failed with %v", argc, result);
    return 1;
  } else if (is_integer(result)) {
    // This is the expected behavior: main returns an integer which we'll use
    // as the exit code.
    return get_integer_value(result);
  } else {
    WARN("try_main(%i, *) returned an unexpected value: %v", argc, result);
    return 0;
  }
}

value_t Main::try_main(int argc, const char *argv[]) {
  neutrino::RuntimeConfig config;
  config.semispace_size_bytes = 10 * kMB;

  ChorusService service;
  neutrino::Runtime runtime;
  runtime.add_service(&service);
  runtime.initialize(&config);

  TRY(Chorus::load_library(runtime));
  return Chorus::run_main(runtime);
}

value_t Chorus::load_library(neutrino::Runtime &runtime) {
  // Load the basic library.
  in_stream_t *stream = byte_in_stream_open(kNLibData, kNLibSize);
  TRY_FINALLY {
    E_TRY(runtime_load_library_from_stream(*runtime, stream, nothing()));
    E_RETURN(success());
  } FINALLY {
    byte_in_stream_destroy(stream);
  } YRT
}

value_t Chorus::run_main(neutrino::Runtime &runtime) {
  in_stream_t *stream = byte_in_stream_open(kNMainData, kNMainSize);
  CREATE_SAFE_VALUE_POOL(*runtime, 4, pool);
  TRY_FINALLY
    E_TRY_DEF(input, read_stream_to_blob(*runtime, stream));
    E_TRY_DEF(program, safe_runtime_plankton_deserialize(*runtime, protect(pool, input)));
    E_RETURN(safe_runtime_execute_syntax(*runtime, protect(pool, program)));
  FINALLY
    byte_in_stream_destroy(stream);
    DISPOSE_SAFE_VALUE_POOL(pool);
  YRT
}

neutrino::Maybe<> ChorusService::bind(neutrino::ForeignServiceBinder *config) {
  config->set_namespace_name("chorus");
  config->set_display_name("Chorus");
  config->add_method("list_shells", tclib::new_callback(&ChorusService::list_shells, this));
  return neutrino::Maybe<>::with_value();
}

void ChorusService::list_shells(neutrino::ServiceRequest *request) {
  std::vector<std::string> shells;
  Main::list_shells(&shells);
  plankton::Array result = request->factory()->new_array(shells.size());
  for (size_t i = 0; i < shells.size(); i++) {
    std::string shell = shells[i];
    result.add(request->factory()->new_string(shell.c_str(), shell.length()));
  }
  request->fulfill(result);
}

#ifdef IS_GCC
#  include "chorus-posix.cc"
#endif

#ifdef IS_MSVC
#  include "chorus-msvc.cc"
#endif

#define DATA_SIZE_NAME Chorus::kNLibSize
#define DATA_NAME Chorus::kNLibData
#include "nlib.c"
#undef DATA_SIZE_NAME
#undef DATA_NAME

#define DATA_SIZE_NAME Chorus::kNMainSize
#define DATA_NAME Chorus::kNMainData
#include "nmain.c"
#undef DATA_SIZE_NAME
#undef DATA_NAME
