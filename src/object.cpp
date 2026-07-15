#include "object.hpp"
#include "picosha2.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <zlib.h>

namespace {
std::vector<uint8_t> zlib_compress(const std::vector<uint8_t> &data) {
  uLongf dest_len = compressBound(static_cast<uLong>(data.size()));
  std::vector<uint8_t> result(dest_len);

  int ret = ::compress(result.data(), &dest_len, data.data(),
                       static_cast<uLong>(data.size()));

  if (ret != Z_OK) {
    throw std::runtime_error("zlib compression failed with code: " +
                             std::to_string(ret));
  }

  result.resize(dest_len);
  return result;
}

std::vector<uint8_t> zlib_decompress(const std::vector<uint8_t> &data) {
  z_stream stream{};
  stream.next_in = const_cast<Bytef *>(data.data());
  stream.avail_in = static_cast<uInt>(data.size());

  if (inflateInit(&stream) != Z_OK) {
    throw std::runtime_error("Failed to initialize zlib decompression");
  }

  std::vector<uint8_t> result;
  uint8_t buffer[4096];

  int ret;
  do {
    stream.next_out = buffer;
    stream.avail_out = sizeof(buffer);

    ret = inflate(&stream, Z_NO_FLUSH);

    if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
      inflateEnd(&stream);
      throw std::runtime_error("zlib decompression error: " +
                               std::to_string(ret));
    }

    size_t have = sizeof(buffer) - stream.avail_out;
    result.insert(result.end(), buffer, buffer + have);
  } while (ret != Z_STREAM_END);

  inflateEnd(&stream);
  return result;
}
} // namespace

GeetBlob::GeetBlob(const std::vector<uint8_t> &data) { blob_data = data; }

std::vector<uint8_t> GeetBlob::serialize() const { return blob_data; }

void GeetBlob::deserialize(const std::vector<uint8_t> &data) {
  blob_data = data;
}

std::string GeetBlob::type() const { return "blob"; }

std::unique_ptr<GeetObject> object_read(const GeetRepository &repo,
                                        const std::string &sha) {
  fs::path rel_path = fs::path("objects") / sha.substr(0, 2) / sha.substr(2);
  fs::path path = repo_file(repo, rel_path);

  if (!fs::is_regular_file(path)) {
    return nullptr;
  }

  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open object file: " + path.string());
  }

  auto size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> compressed(static_cast<size_t>(size));
  file.read(reinterpret_cast<char *>(compressed.data()),
            static_cast<std::streamsize>(size));

  std::vector<uint8_t> raw = zlib_decompress(compressed);

  auto space_it = std::find(raw.begin(), raw.end(), ' ');
  if (space_it == raw.end()) {
    throw std::runtime_error("Malformed object " + sha +
                             ": no space in header");
  }

  std::string fmt(raw.begin(), space_it);

  auto null_it = std::find(space_it, raw.end(), '\0');
  if (null_it == raw.end()) {
    throw std::runtime_error("Malformed object " + sha + ": no null in header");
  }

  std::string size_str(space_it + 1, null_it);
  int expected_size = std::stoi(size_str);

  int actual_size = static_cast<int>(raw.end() - (null_it + 1));
  if (expected_size != actual_size) {
    throw std::runtime_error("Malformed object " + sha + ": bad length. " +
                             "Expected " + std::to_string(expected_size) +
                             ", got " + std::to_string(actual_size));
  }

  std::vector<uint8_t> content(null_it + 1, raw.end());

  if (fmt == "blob") {
    auto obj = std::make_unique<GeetBlob>();
    obj->deserialize(content);
    return obj;
  }

  throw std::runtime_error("Unknown type " + fmt + " for object " + sha);
}

std::string object_write(GeetObject &obj, const GeetRepository *repo) {
  std::vector<uint8_t> data = obj.serialize();
  std::string header = obj.type() + " " + std::to_string(data.size());
  std::vector<uint8_t> result;
  result.reserve(header.size() + 1 + data.size());

  result.insert(result.end(), header.begin(), header.end());
  result.push_back('\0');
  result.insert(result.end(), data.begin(), data.end());

  std::string sha = picosha2::hash256_hex_string(result.begin(), result.end());

  if (repo) {
    fs::path rel_path = fs::path("objects") / sha.substr(0, 2) / sha.substr(2);

    fs::path path = repo_file(*repo, rel_path, true);

    if (!fs::exists(path)) {
      std::vector<uint8_t> compressed = zlib_compress(result);

      std::ofstream out(path, std::ios::binary);
      if (!out.is_open()) {
        throw std::runtime_error("Could not write object: " + path.string());
      }
      out.write(reinterpret_cast<const char *>(compressed.data()),
                static_cast<std::streamsize>(compressed.size()));
    }
  }

  return sha;
}

std::string object_find(const GeetRepository &repo, const std::string &name,
                        const std::string &fmt, bool follow) {
  (void)repo;
  (void)fmt;
  (void)follow;

  return name;
}
