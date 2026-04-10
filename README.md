# Runtime-code-Obf

Runtime-code-Obf is a small header-only C++20 MVP that obfuscates string literals at compile time and verifies the generated binary does not retain selected plaintext secrets.

## What the MVP includes

- `include/runtime_obf/obfuscated_string.hpp` — header-only compile-time string obfuscation primitive
- `examples/basic.cpp` — simple executable that decrypts and prints several obfuscated literals
- `tests/obfuscated_string_tests.cpp` — unit tests that cover compile-time and run-time behavior
- `scripts/check_plaintext.py` — binary inspection script that fails if configured plaintext strings are present in the built executable

## Quick start

```cpp
#include "runtime_obf/obfuscated_string.hpp"

constexpr auto secret = RUNTIME_OBF("compile-time secret");
const auto plaintext = secret.decrypt();
```

The `RUNTIME_OBF("...")` macro bakes encrypted bytes into the binary and reconstructs the plaintext at runtime when you call `decrypt()` or `copy_to(...)`.

## Build and test

```powershell
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The test suite contains:

1. **Unit tests** for compile-time construction and run-time decryption
2. **Plaintext scan** that reads the built example executable and fails if `hunter2`, `compile-time secret`, or `api-key-123` still appear in the binary

## Caveats

- This is **obfuscation**, not cryptography. It raises the bar for static string scraping but does not protect secrets once your program decrypts them in memory.
- The plaintext-scan guarantee is only meaningful for the exact compiler/configuration you build and test. Debug info, PDBs, LTO differences, sanitizer builds, or a different compiler can change the result.
- The default macro uses `__COUNTER__` and `__LINE__` to derive unique per-call seeds. That is widely supported by modern compilers, but `__COUNTER__` is still a compiler extension.

## Project layout

```text
.
├── CMakeLists.txt
├── README.md
├── examples/basic.cpp
├── include/runtime_obf/obfuscated_string.hpp
├── scripts/check_plaintext.py
└── tests/obfuscated_string_tests.cpp
```
