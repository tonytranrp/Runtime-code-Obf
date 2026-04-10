#include <array>
#include <iostream>

#include "runtime_obf/obfuscated_string.hpp"

int main() {
  constexpr auto password = RUNTIME_OBF("hunter2");
  constexpr auto secret = RUNTIME_OBF("compile-time secret");
  constexpr auto api_key = RUNTIME_OBF("api-key-123");

  const auto password_text = password.decrypt_scoped();
  const auto secret_text = secret.decrypt_scoped();
  const auto api_key_text = api_key.decrypt_scoped();

  std::array<char, decltype(password)::encoded_size()> buffer{};
  password.copy_to(buffer);

  std::cout << "Password: " << password_text.view() << '\n'
            << "Secret: " << secret_text.view() << '\n'
            << "API key: " << api_key_text.view() << '\n'
            << "Buffer: " << buffer.data() << '\n';

  return 0;
}
