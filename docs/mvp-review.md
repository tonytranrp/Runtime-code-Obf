# Compile-Time String Obfuscation MVP Review Guide

This document is the review rubric for the first `Runtime-code-obf` milestone.
It is intentionally narrow: the MVP is about **compile-time string obfuscation**
for a modern, header-only C++ library. It is **not** a promise of general
anti-reversing protection, encryption, or tamper-proofing.

## MVP goals

- Keep the library header-only and dependency-free.
- Provide a small public API for obfuscating string literals at compile time.
- Decode only when the caller explicitly requests plaintext at runtime.
- Make the verification story part of the deliverable, not a follow-up task.
- Document caveats clearly whenever a compiler, build mode, or usage pattern can
  still leave recognizable plaintext in the binary.

## Code quality checklist

Review the implementation against the following requirements before merging:

### Public API

- The API surface is intentionally small and obvious to consume.
- Public names are stable, readable, and consistent with normal C++ library
  conventions.
- Usage does not require global mutable state or hidden initialization steps.
- The library does not force heap allocation for the common decode path.

### Compile-time behavior

- Obfuscation happens in constant evaluation for string literals.
- Encoded storage is derived from the source literal without keeping an
  additional plaintext copy in static storage.
- Any per-literal keying material is deterministic for a build and scoped to the
  literal instance or call site.
- The implementation avoids undefined behavior and does not depend on
  implementation-specific object layout tricks when a standard alternative is
  available.

### Runtime behavior

- Runtime decode is explicit and easy to reason about.
- Decoded buffers have a clear lifetime contract.
- Repeated decode operations do not accidentally keep long-lived plaintext
  copies around unless the caller explicitly stores them.
- The library keeps the common path allocation-free when practical.

### Portability and maintenance

- The core implementation is valid modern C++ and does not depend on platform
  APIs.
- Compiler-specific branches are isolated and justified.
- The implementation is easy to inspect in a debugger and simple enough to test
  directly.
- Comments explain *why* a tricky step exists, not just *what* the code does.

## Binary verification expectations

The implementation is only acceptable if it ships with an evidence-driven
verification loop:

- Build at least one Release example or test binary.
- Search the built artifact with a binary-inspection tool such as `strings`,
  `llvm-strings`, or `dumpbin`.
- Confirm that representative protected literals do not appear as obvious
  plaintext in the final artifact.
- If any compiler, configuration, or misuse case still leaks plaintext, the docs
  must describe that caveat explicitly.

## Caveats that must stay explicit

Do **not** overclaim the protection level. The docs and review notes must keep
the following caveats visible:

- This is obfuscation, not cryptographic secrecy.
- Plaintext will normally exist in memory after a caller decodes it.
- Aggressive debugging, sanitizer, or instrumentation builds may change emitted
  artifacts and can weaken the result.
- Compiler optimizations, constant merging, or accidental API misuse may still
  expose recognizable data if the implementation is careless.

## Merge gate summary

The MVP review should fail if any of the following is true:

- The public API is broader than needed for string-literal obfuscation.
- Plaintext remains trivially discoverable in Release artifacts without being
  called out as a caveat.
- The implementation relies on unclear lifetime rules for decoded strings.
- The docs describe the library as "secure" or "undetectable" without evidence.
