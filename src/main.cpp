#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "repository.hpp"
#include <CLI/CLI.hpp>

namespace fs = std::filesystem;

void cmd_init(const std::string &path) {
  try {
    repo_create(path);
    std::cout << "Initializing empty Geet repository in: " << fs::absolute(path)
              << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Fatal: " << e.what()
              << "\n"; // remember to get an explanation for this later.
  }
}

int main(int argc, char **argv) {
  CLI::App app{"Geet: A light, educational Git implementation in C++"};

  app.require_subcommand(1);

  auto *init_sub =
      app.add_subcommand("init", "Initialize a new, empty repository.");

  std::string init_path = ".";
  init_sub->add_option("path", init_path, "Where to create the repository.")
      ->default_val(".");

  init_sub->callback([&init_path]() { cmd_init(init_path); });

  CLI11_PARSE(app, argc, argv);

  return 0;
}