#include <array>
#include <iostream>

#include "runtime_obf/obfuscated_string.hpp"

int main() {
  constexpr auto password = RUNTIME_OBF("hunter2");
  constexpr auto secret = RUNTIME_OBF("compile-time secret");
  constexpr auto api_key = RUNTIME_OBF("api-key-123");

  const auto password_text = password.decrypt();
  const auto secret_text = secret.decrypt();
  const auto api_key_text = api_key.decrypt();

  std::array<char, decltype(password)::encoded_size()> buffer{};
  password.copy_to(buffer);

  std::cout << "Password: " << password_text << '\n'
            << "Secret: " << secret_text << '\n'
            << "API key: " << api_key_text << '\n'
            << "Buffer: " << buffer.data() << '\n';

  return (password_text == "hunter2" && secret_text == "compile-time secret"
          && api_key_text == "api-key-123")
             ? 0
             : 1;
}
