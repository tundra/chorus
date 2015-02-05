//- Copyright 2015 the Neutrino authors (see AUTHORS).
//- Licensed under the Apache License, Version 2.0 (see LICENSE).

void Main::list_shells(std::vector<std::string> *out) {
  // TODO: use whatever the standard hook is to replace the default shell,
  //   probably some registry key, and look that up. Try installing powershell
  //   and see what that does to become the default. If it doesn't introduce
  //   a registry key.
  out->push_back("cmd.exe");
}
