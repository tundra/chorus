//- Copyright 2015 the Neutrino authors (see AUTHORS).
//- Licensed under the Apache License, Version 2.0 (see LICENSE).

#include "chorus.hh"
#include "plankton-inl.hh"

BEGIN_C_INCLUDES
#include "format.h"
#include "include/neutrino.h"
#include "plugin.h"
#include "runtime.h"
#include "safe-inl.h"
#include "try-inl.h"
#include "utils/log.h"
END_C_INCLUDES

using namespace chorus;
using namespace plankton;

// Holds data used for interacting with neutrino.
class ChorusPlugin {
public:
  ChorusPlugin();
  static value_t test_method(builtin_arguments_t *args);

  // Initialize the runtime config, before the runtime has been constructed.
  void init_config(neu_runtime_config_t *config);

  // Initialize the runtime itself.
  value_t init_runtime(runtime_t *runtime);

  // Run the main program in the given runtime.
  value_t run_main(runtime_t *runtime);

private:
  // The library data.
  static const size_t kNLibSize;
  static const byte_t kNLibData[];

  // The main program data.
  static const size_t kNMainSize;
  static const byte_t kNMainData[];

  c_object_info_t chorus_;
  const c_object_info_t *plugins_;
  c_object_method_t methods_[1];
};

int Main::main(int argc, const char *argv[]) {
  value_t result = try_main(argc, argv);
  if (is_condition(result)) {
    ERROR("try_main(%i, *) failed with %v", argc, result);
    return 1;
  } else if (is_integer(result)) {
    // This is the expected behavior: main returns an integer which we'll use
    // as the exit code.
    return (int) get_integer_value(result);
  } else {
    WARN("try_main(%i, *) returned an unexpected value: %v", argc, result);
    return 0;
  }
}

value_t Main::try_main(int argc, const char *argv[]) {
  extended_runtime_config_t config = *extended_runtime_config_get_default();
  ChorusPlugin plugins;
  plugins.init_config(&config.base);

  runtime_t *runtime = NULL;
  TRY(new_runtime(&config, &runtime));
  TRY_FINALLY {
    E_TRY(plugins.init_runtime(runtime));
    E_TRY_DEF(result, plugins.run_main(runtime));
    E_RETURN(result);
  } FINALLY {
    TRY(delete_runtime(runtime, dfDefault));
  } YRT
}

ChorusPlugin::ChorusPlugin()
  : plugins_(&chorus_) {
  c_object_info_reset(&chorus_);
  c_object_info_set_tag(&chorus_, new_integer(0xBAF));
  c_object_method_t method = BUILTIN_METHOD("test", 0, test_method);
  methods_[0] = method;
  c_object_info_set_methods(&chorus_, methods_, 1);
}

void ChorusPlugin::init_config(neu_runtime_config_t *config) {
  config->semispace_size_bytes = 10 * kMB;
  config->plugin_count = 1;
  config->plugins = (const void**) &plugins_;
}

value_t ChorusPlugin::init_runtime(runtime_t *runtime) {
  // Load the basic library.
  in_stream_t *stream = byte_in_stream_open(kNLibData, kNLibSize);
  TRY_FINALLY {
    E_TRY(runtime_load_library_from_stream(runtime, stream, nothing()));
    E_RETURN(success());
  } FINALLY {
    byte_in_stream_destroy(stream);
  } YRT
}

value_t ChorusPlugin::run_main(runtime_t *runtime) {
  in_stream_t *stream = byte_in_stream_open(kNMainData, kNMainSize);
  CREATE_SAFE_VALUE_POOL(runtime, 4, pool);
  TRY_FINALLY {
    E_TRY_DEF(input, read_stream_to_blob(runtime, stream));
    E_TRY_DEF(program, safe_runtime_plankton_deserialize(runtime, protect(pool, input)));
    E_RETURN(safe_runtime_execute_syntax(runtime, protect(pool, program)));
  } FINALLY {
    byte_in_stream_destroy(stream);
    DISPOSE_SAFE_VALUE_POOL(pool);
  } YRT
}

value_t ChorusPlugin::test_method(builtin_arguments_t *args) {
  return new_integer(100);
}

#ifdef IS_GCC
#  include "chorus-posix.cc"
#endif


#ifdef IS_MSVC
#  include "chorus-msvc.cc"
#endif

#define DATA_SIZE_NAME ChorusPlugin::kNLibSize
#define DATA_NAME ChorusPlugin::kNLibData
#include "nlib.c"
#undef DATA_SIZE_NAME
#undef DATA_NAME

#define DATA_SIZE_NAME ChorusPlugin::kNMainSize
#define DATA_NAME ChorusPlugin::kNMainData
#include "nmain.c"
#undef DATA_SIZE_NAME
#undef DATA_NAME
