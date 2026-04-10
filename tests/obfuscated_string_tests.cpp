#include <cstdlib>
#include <iostream>
#include <string>

#include "runtime_obf/obfuscated_string.hpp"

namespace {

constexpr auto kSecret = RUNTIME_OBF("compile-time secret");

constexpr bool encrypted_differs_from_plaintext() {
  constexpr char literal[] = "compile-time secret";
  for (std::size_t index = 0; index < kSecret.encoded_size(); ++index) {
    if (kSecret.encrypted_bytes()[index] != static_cast<unsigned char>(literal[index])) {
      return true;
    }
  }
  return false;
}

void expect(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAILED: " << message << '\n';
    std::exit(1);
  }
}

}  // namespace

int main() {
  static_assert(kSecret.string_size() == 19);
  static_assert(encrypted_differs_from_plaintext(), "Encrypted bytes should differ from the literal.");

  constexpr auto decrypted = kSecret.decrypt_array();
  static_assert(decrypted[0] == 'c');
  static_assert(decrypted[12] == ' ');
  static_assert(decrypted[18] == 't');
  static_assert(decrypted[19] == '\0');

  expect(kSecret.decrypt() == "compile-time secret", "decrypt() should round-trip the literal");

  char buffer[decltype(kSecret)::encoded_size()]{};
  kSecret.copy_to(buffer);
  expect(std::string(buffer) == "compile-time secret", "copy_to() should write a null-terminated buffer");

  constexpr auto empty = RUNTIME_OBF("");
  static_assert(empty.string_size() == 0);
  expect(empty.decrypt().empty(), "empty literals should decrypt to empty strings");

  constexpr auto other = RUNTIME_OBF("hunter2");
  expect(other.decrypt() == "hunter2", "multiple obfuscated strings should decrypt independently");

  std::cout << "runtime_obf tests passed\n";
  return 0;
}
