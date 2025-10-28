#include "DiffInk/Utils/XxhStl.h"

namespace diffink {

XXH32_hash_t xxhVector32(const std::vector<XXH32_hash_t> &data) noexcept {
  return XXH32(static_cast<const void *>(data.data()),
               data.size() * sizeof(XXH32_hash_t), 0);
}

XXH64_hash_t xxhVector64(const std::vector<XXH64_hash_t> &data) noexcept {
  return XXH64(static_cast<const void *>(data.data()),
               data.size() * sizeof(XXH64_hash_t), 0);
}

XXH128_hash_t xxhVector128(const std::vector<XXH128_hash_t> &data) noexcept {
  return XXH128(static_cast<const void *>(data.data()),
                data.size() * sizeof(XXH128_hash_t), 0);
}

XXH32_hash_t xxhString32(const std::string &data) noexcept {
  return XXH32(static_cast<const void *>(data.data()),
               data.size() * sizeof(char), 0);
}

XXH64_hash_t xxhString64(const std::string &data) noexcept {
  return XXH64(static_cast<const void *>(data.data()),
               data.size() * sizeof(char), 0);
}

XXH128_hash_t xxhString128(const std::string &data) noexcept {
  return XXH128(static_cast<const void *>(data.data()),
                data.size() * sizeof(char), 0);
}

XXH32_hash_t xxhString32(const char *Offset, std::size_t Size) noexcept {
  return XXH32(static_cast<const void *>(Offset), Size * sizeof(char), 0);
}

} // namespace diffink