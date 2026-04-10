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
const auto scoped = secret.decrypt_scoped();
```

The `RUNTIME_OBF("...")` macro bakes encrypted bytes into the binary and reconstructs the plaintext at runtime when you call `decrypt()`, `decrypt_scoped()`, or `copy_to(...)`.

- `decrypt()` returns a `std::string` for convenience.
- `decrypt_scoped()` returns a small RAII buffer that zeroes its own storage on destruction or when you call `wipe()`.
- `copy_to(...)` lets you decode directly into caller-owned storage and avoid a heap allocation.

## Build and test

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

On Windows, run those commands from a Visual Studio Developer Command Prompt (or after calling `VsDevCmd.bat`) so `cl.exe`, `link.exe`, and `ninja` are available.

The test suite contains:

1. **Unit tests** for compile-time construction and run-time decryption
2. **Plaintext scan** that reads the built example executable and fails if `hunter2`, `compile-time secret`, or `api-key-123` still appear in the binary
3. **MSVC dumpbin scan** that inspects generated `/rawdata` and `/disasm` reports for the same plaintext literals

## CMake consumption

The library is header-only, so CPM/FetchContent users can consume it directly as
an `INTERFACE` target without pulling in third-party dependencies:

```cmake
CPMAddPackage("gh:tonytranrp/Runtime-code-Obf#main")
target_link_libraries(your_target PRIVATE runtime_obf::runtime_obf)
```

Install-based consumers can also use the exported package target after
`cmake --install`:

```cmake
find_package(runtime_obf CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE runtime_obf::runtime_obf)
```

## Caveats

- This is **obfuscation**, not cryptography. It raises the bar for static string scraping but does not protect secrets once your program decrypts them in memory.
- `decrypt_scoped()` only clears the storage it owns. If the caller copies the plaintext into another buffer, string, logger, or crash dump, that extra copy is outside the library's control.
- The plaintext-scan guarantee is only meaningful for the exact compiler/configuration you build and test. Debug info, PDBs, LTO differences, sanitizer builds, or a different compiler can change the result.
- The default macro uses `__COUNTER__` and `__LINE__` to derive unique per-call seeds. That is widely supported by modern compilers, but `__COUNTER__` is still a compiler extension.
- The encrypted byte arrays and decode logic are still present and recoverable to a determined reverse engineer. The library aims to block trivial plaintext scraping, not to make recovery impossible.

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
