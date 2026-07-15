#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "object.learn.hpp"
#include "repository.hpp"
#include <CLI/CLI.hpp>

namespace fs = std::filesystem;

void cmd_init(const std::string &path) {
  try {
    repo_create(path);
    std::cout << "Initializing empty Geet repository in: " << fs::absolute(path)
              << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Fatal: " << e.what() << "\n";
  }
}

void cmd_cat_file(const std::string &type, const std::string &object_sha) {
  try {
    auto repo = repo_find();
    if (!repo) {
      std::cerr << "Fatal: not a geet repositiry\n";
      return;
    }

    std::string sha = object_find(*repo, object_sha, type);

    auto obj = object_read(*repo, sha);
    if (!obj) {
      std::cerr << "Fatal: object not found: " << sha << "\n";
      return;
    }

    std::vector<uint8_t> data = obj->serialize();

    std::cout.write(reinterpret_cast<const char *>(data.data()),
                    static_cast<std::streamsize>(data.size()));

  } catch (const std::exception &e) {
    std::cerr << "Fatal: " << e.what() << "\n";
  }
}

// hash object
void cmd_hash_object(const std::string &file_path, const std::string &type,
                     bool write_flag) {
  try {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
      std::cerr << "Fatal: could not open file: " << file_path << "\n";
      return;
    }

    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(static_cast<size_t>(size));
    file.read(reinterpret_cast<char *>(data.data()),
              static_cast<std::streamsize>(size));

    std::unique_ptr<GeetObject> obj;
    if (type == "blob") {
      obj = std::make_unique<GeetBlob>(data);
    } else {
      std::cerr << "Fatal: unsupported object type: " << type << "\n";
      return;
    }

    const GeetRepository *repo_ptr = nullptr;
    std::optional<GeetRepository> repo;

    if (write_flag) {
      repo = repo_find();
      if (!repo) {
        std::cerr << "Fatal: not a geet repository\n";
        return;
      }
      repo_ptr = &(*repo);
    }

    std::string sha = object_write(*obj, repo_ptr);
    std::cout << sha << "\n";

  } catch (const std::exception &e) {
    std::cerr << "Fatal: " << e.what() << "\n";
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