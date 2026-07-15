#pragma once

#include "repository.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class GeetObject {
public:
  virtual ~GeetObject() = default;
  virtual std::vector<uint8_t> serialize() const = 0;
  virtual void deserialize(const std::vector<uint8_t> &data) = 0;
  virtual std::string type() const = 0;
};

class GeetBlob : public GeetObject {
public:
  std::vector<uint8_t> blob_data;
  GeetBlob() = default;

  explicit GeetBlob(const std::vector<uint8_t> &data);

  std::vector<uint8_t> serialize() const override;
  void deserialize(const std::vector<uint8_t> &data) override;
  std::string type() const override;
};

std::unique_ptr<GeetObject> object_read(const GeetRepository &repo,
                                        const std::string &sha);
std::string object_write(GeetObject &obj, const GeetRepository *repo = nullptr);
std::string object_find(const GeetRepository &repo, const std::string &name,
                        const std::string &fmt = "", bool follow = true);