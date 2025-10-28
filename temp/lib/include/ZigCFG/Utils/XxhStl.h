#ifndef DIFFINK_UTILS_XXHSTL_H
#define DIFFINK_UTILS_XXHSTL_H

#include <string>
#include <vector>
#include <xxHash/xxh3.h>

namespace diffink {

XXH32_hash_t xxhVector32(const std::vector<XXH32_hash_t> &data) noexcept;

XXH64_hash_t xxhVector64(const std::vector<XXH64_hash_t> &data) noexcept;

XXH128_hash_t xxhVector128(const std::vector<XXH128_hash_t> &data) noexcept;

XXH32_hash_t xxhString32(const std::string &data) noexcept;

XXH64_hash_t xxhString64(const std::string &data) noexcept;

XXH128_hash_t xxhString128(const std::string &data) noexcept;

XXH32_hash_t xxhString32(const char *Offset, std::size_t Size) noexcept;

} // namespace diffink

#endif // DIFFINK_UTILS_XXHSTL_H