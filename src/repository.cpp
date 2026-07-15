#include "repository.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

GeetRepository::GeetRepository(const fs::path &path, bool force) {
  worktree = fs::absolute(path);
  gitdir = worktree / ".geet";

  if (!force) {
    if (!fs::is_directory(gitdir)) {
      throw std::runtime_error("Not a git repository: " + gitdir.string());
    }

    fs::path config_file = repo_file(*this, "config");
    if (fs::exists(config_file)) {
      std::ifstream ifs(config_file);
      if (!ifs.is_open()) {
        throw std::runtime_error("Could not open config file: " +
                                 config_file.string());
      }
      conf.parse(ifs);
    } else {
      throw std::runtime_error("Configuration file missing in " +
                               gitdir.string());
    }

    auto &core_sec = conf.sections["core"];
    auto it = core_sec.find("repositoryformatversion");
    if (it == core_sec.end()) {
      throw std::runtime_error(
          "Configuration file missing 'repositoryformatversion' in [core]");
    }
    std::string version_str = it->second;
    int version = std::stoi(version_str);
    if (version != 0) {
      throw std::runtime_error("Unsupported repositoryformatversion: " +
                               version_str);
    }
  }
}

// Helper functions

fs::path repo_path(const GeetRepository &repo, const fs::path &relative_path) {
  return repo.gitdir / relative_path;
}

fs::path repo_file(const GeetRepository &repo, const fs::path &relative_path,
                   bool mkdir) {
  fs::path path = repo_path(repo, relative_path);
  if (mkdir) {
    fs::create_directories(path.parent_path());
  }
  return path;
}

fs::path repo_dir(const GeetRepository &repo, const fs::path &relative_path,
                  bool mkdir) {
  fs::path path = repo_path(repo, relative_path);

  if (fs::exists(path)) {
    if (fs::is_directory(path)) {
      return path;
    } else {
      throw std::runtime_error("Not a directory: " + path.string());
    }
  }

  if (mkdir) {
    fs::create_directories(path);
    return path;
  }
  return path;
}

// Repository operations

GeetRepository repo_create(const fs::path &path) {
  GeetRepository repo(path, true);

  if (fs::exists(repo.worktree)) {
    if (!fs::is_directory(repo.worktree)) {
      throw std::runtime_error(path.string() + " is not a directory!");
    }

    if (fs::exists(repo.gitdir) && !fs::is_empty(repo.gitdir)) {
      throw std::runtime_error(repo.gitdir.string() + " is not empty");
    }
  } else {
    fs::create_directories(repo.worktree);
  }

  repo_dir(repo, "branches", true);
  repo_dir(repo, "objects", true);
  repo_dir(repo, "refs/heads", true);
  repo_dir(repo, "refs/tags", true);

  fs::path description_path = repo_file(repo, "description");
  std::ofstream desc_file(description_path);
  if (!desc_file.is_open()) {
    throw std::runtime_error("Could not write: " + description_path.string());
  }
  desc_file << "unnamed repository; edit this file 'description' to name the "
               "repository.\n";
  desc_file.close();

  fs::path head_path = repo_file(repo, "HEAD");
  std::ofstream head_file(head_path);
  if (!head_file.is_open()) {
    throw std::runtime_error("Could not write: " + head_path.string());
  }

  head_file << "ref: refs/heads/master\n";
  head_file.close();

  fs::path config_path = repo_file(repo, "config");
  std::ofstream config_file(config_path);
  if (!config_file.is_open()) {
    throw std::runtime_error("Could not write: " + config_path.string());
  }

  inipp::Ini<char> default_conf;

  default_conf.sections["core"]["repositoryformatversion"] = "0";
  default_conf.sections["core"]["filemode"] = "false";
  default_conf.sections["core"]["bare"] = "false";
  default_conf.generate(config_file);

  return repo;
}

std::optional<GeetRepository> repo_find(const fs::path &path, bool required) {
  fs::path abs_path = fs::absolute(path);

  if (fs::is_directory(abs_path / ".geet")) {
    return GeetRepository(abs_path);
  }

  fs::path parent = abs_path.parent_path();

  if (parent == abs_path) {
    if (required) {
      throw std::runtime_error(
          "Not a geet repository (or any of the parent directories): .geet");
    }
    return std::nullopt;
  }

  return repo_find(parent, required);
}