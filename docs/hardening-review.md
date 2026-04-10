# Hardening Review - verification depth and runtime exposure

## Baseline findings

The verified MVP already blocked the most obvious failure mode: plaintext
protected literals were not visible in the Release executable or object file as
raw UTF-8 / UTF-16LE strings.

The next most realistic recovery paths are still:

1. **Recognizable decode code** - the binary still contains a byte-wise decode
   routine and encrypted byte arrays. In the checked MSVC object-file disassembly,
   `decrypt_scoped`, `key_at`, and `secure_clear` remain visible as named helper
   symbols, and the `decrypt_scoped` path materializes SIMD constants used by the
   key schedule.
2. **Static encrypted layouts** - encrypted bytes are still embedded in the
   artifact and remain recoverable with manual reverse engineering.
3. **Runtime plaintext lifetime** - convenience APIs can keep decoded material
   alive longer than necessary once the caller reconstructs it.

## Accepted changes

### 1. Stronger verification lane

- Added a `runtime_obf.dumpbin_scan` CTest lane on MSVC builds.
- The lane generates `dumpbin /rawdata` and `dumpbin /disasm` reports for the
  example executable and rejects protected literals if they appear in either the
  binary or the generated text reports.
- Extended `scripts/check_plaintext.py` so it can scan binary bytes, printable
  ASCII segments, and optional text reports.

**Why accepted:** this is a clean, reproducible extension of the existing
verification lane and catches regressions earlier than manual inspection alone.

### 2. Scoped plaintext helper

- Added `runtime_obf::scoped_plaintext<N>`
- Added `obfuscated_string::decrypt_scoped()`
- Added explicit `wipe()` support for the scoped plaintext buffer

**Why accepted:** it reduces unnecessary heap allocation and gives callers a
small, explicit tool for limiting how long library-owned plaintext stays in
memory.

## Rejected ideas

### Aggressive data-layout randomization

Rejected because it would add complexity without changing the core truth that
encrypted bytes still exist in the artifact and can be recovered by a dedicated
analyst.

### More elaborate anti-disassembly tricks

Rejected because they would be compiler-specific, fragile, and likely to hurt
maintainability long before they produced a trustworthy cross-toolchain win.

### Claiming zeroization guarantees beyond owned storage

Rejected because it would be dishonest. The library can clear the scoped buffer
it owns, but it cannot clear copies the caller makes elsewhere.

## Current honest boundary

After this hardening pass, the project is better at:

- rejecting obvious plaintext regressions in built MSVC Release artifacts,
- documenting the remaining recovery paths, and
- reducing plaintext lifetime when the caller chooses the scoped API.

It still does **not** prevent:

- recovery through reverse engineering of encrypted static data plus decode
  logic,
- runtime memory inspection after decode, or
- compiler/configuration-specific regressions that were not part of the checked
  build lane.
