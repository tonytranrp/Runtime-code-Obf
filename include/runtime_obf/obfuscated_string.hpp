#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace runtime_obf {

namespace detail {

[[nodiscard]] consteval std::uint64_t splitmix64(std::uint64_t value) noexcept {
  value += 0x9E3779B97F4A7C15ull;
  value = (value ^ (value >> 30u)) * 0xBF58476D1CE4E5B9ull;
  value = (value ^ (value >> 27u)) * 0x94D049BB133111EBull;
  return value ^ (value >> 31u);
}

[[nodiscard]] consteval std::uint64_t make_seed(
    std::uint64_t line,
    std::uint64_t counter) noexcept {
  return splitmix64((line << 32u) ^ counter ^ 0xA5A5A5A5F00DF00Dull);
}

[[nodiscard]] constexpr std::uint8_t key_at(
    std::uint64_t seed,
    std::size_t index) noexcept {
  auto mixed = seed + (0x9E3779B97F4A7C15ull * (static_cast<std::uint64_t>(index) + 1ull));
  mixed ^= mixed >> 33u;
  mixed *= 0xFF51AFD7ED558CCDull;
  mixed ^= mixed >> 33u;
  mixed *= 0xC4CEB9FE1A85EC53ull;
  mixed ^= mixed >> 33u;
  return static_cast<std::uint8_t>(mixed & 0xFFu);
}

}  // namespace detail

template <std::size_t N, std::uint64_t Seed>
class obfuscated_string final {
 public:
  using encrypted_storage = std::array<std::uint8_t, N>;
  using decrypted_storage = std::array<char, N>;

  consteval explicit obfuscated_string(const char (&literal)[N]) noexcept {
    for (std::size_t index = 0; index < N; ++index) {
      encrypted_[index] = static_cast<std::uint8_t>(literal[index]) ^ detail::key_at(Seed, index);
    }
  }

  [[nodiscard]] constexpr auto decrypt_array() const noexcept -> decrypted_storage {
    decrypted_storage result{};
    for (std::size_t index = 0; index < N; ++index) {
      result[index] = static_cast<char>(encrypted_[index] ^ detail::key_at(Seed, index));
    }
    return result;
  }

  [[nodiscard]] auto decrypt() const -> std::string {
    const auto result = decrypt_array();
    return std::string(result.data(), N - 1u);
  }

  template <std::size_t BufferSize>
  constexpr void copy_to(char (&buffer)[BufferSize]) const noexcept {
    static_assert(BufferSize >= N, "Destination buffer is too small.");
    const auto result = decrypt_array();
    for (std::size_t index = 0; index < N; ++index) {
      buffer[index] = result[index];
    }
  }

  template <std::size_t BufferSize>
  constexpr void copy_to(std::array<char, BufferSize>& buffer) const noexcept {
    static_assert(BufferSize >= N, "Destination buffer is too small.");
    const auto result = decrypt_array();
    for (std::size_t index = 0; index < N; ++index) {
      buffer[index] = result[index];
    }
  }

  [[nodiscard]] constexpr auto encrypted_bytes() const noexcept -> const encrypted_storage& {
    return encrypted_;
  }

  [[nodiscard]] static constexpr auto encoded_size() noexcept -> std::size_t { return N; }
  [[nodiscard]] static constexpr auto string_size() noexcept -> std::size_t { return N - 1u; }

 private:
  encrypted_storage encrypted_{};
};

template <std::uint64_t Seed, std::size_t N>
[[nodiscard]] consteval auto make_obfuscated(const char (&literal)[N]) {
  return obfuscated_string<N, Seed>(literal);
}

}  // namespace runtime_obf

#define RUNTIME_OBF_DETAIL_SEED() \
  (::runtime_obf::detail::make_seed(static_cast<std::uint64_t>(__LINE__), static_cast<std::uint64_t>(__COUNTER__) + 1ull))

#define RUNTIME_OBF(literal) (::runtime_obf::make_obfuscated<RUNTIME_OBF_DETAIL_SEED()>(literal))
