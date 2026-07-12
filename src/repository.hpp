#pragma once

#include "inipp.h"
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct GeetRepository {
  fs::path worktree;
  fs::path gitdir;
  inipp::Ini<char> conf;

  GeetRepository(const fs::path &path, bool force = false);
};

// Utility functions

fs::path repo_path(const GeetRepository &repo, const fs::path &relative_path);

fs::path repo_file(const GeetRepository &repo, const fs::path &relative_path,
                   bool mkdir = false);

fs::path repo_dir(const GeetRepository &repo, const fs::path &relative_path,
                  bool mkdir = false);

// Repository operations

GeetRepository repo_create(const fs::path &path);

std::optional<GeetRepository> repo_find(const fs::path &path = ".",
                                        bool required = true);