# Current Review Findings

This file records the current code-quality review findings for the compile-time
string-obfuscation MVP.

## Strengths

- The public API is intentionally small and easy to explain.
- The encode path is `consteval`, which keeps the MVP aligned with the
  compile-time requirement.
- The implementation is header-only and does not bring in third-party
  dependencies.
- The repo already includes both unit-style coverage and a direct
  binary-inspection script, which is the right verification posture for this
  project.

## Important caveats to keep documented

- The implementation currently supports narrow character string literals only.
- `decrypt()` returns `std::string`, which intentionally trades simplicity for
  an extra plaintext allocation.
- The example executable prints decrypted data to stdout; this is acceptable for
  demonstration, but it means the runtime path is not attempting to keep
  plaintext hidden after decryption.
- The plaintext scan is currently focused on raw UTF-8 and UTF-16LE matches in
  the built artifact. That is a good MVP guardrail, not a complete reverse
  engineering defense.

## Recommended follow-ups after the MVP

These are useful next steps, but they should stay outside the smallest verified
MVP unless the current implementation proves insufficient:

- Add a broader compiler/configuration verification matrix.
- Evaluate whether package export/config support is worth the added CMake
  surface area.
- Consider whether additional literal character types or buffer-oriented APIs
  are worth expanding the public surface.
