# Binary Verification Workflow

`Runtime-code-obf` must ship with a verification path that inspects built
artifacts directly. Passing unit tests is not enough for this MVP; the goal is
to reject obvious plaintext leakage in compiled binaries or to explain the exact
caveat when leakage cannot be avoided in a given configuration.

## What this workflow is trying to prove

For representative protected literals:

- the plaintext does **not** appear in the final Release artifact as an obvious
  static string-table entry, and
- the project documents any remaining caveats honestly.

This workflow does **not** prove resistance against a determined reverse
engineer, dynamic instrumentation, or memory inspection after decode.

## Recommended verification steps

### 1. Configure a Release build

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

If the project later adds presets, prefer the documented Release preset instead.

### 2. Pick a representative artifact

Use one of:

- a minimal example executable that embeds known protected literals, or
- a focused test binary that exercises the obfuscation path.

Prefer a tiny artifact because it is easier to inspect and reason about.

### 3. Search for known plaintext

Search for one or more literals that were intentionally protected.

#### Unix-like environments

```bash
strings -a <artifact> | grep -F "known secret literal"
```

If `grep` finds the literal, treat that as a verification failure until the
implementation or documentation explains the result.

#### LLVM toolchains

```bash
llvm-strings <artifact> | grep -F "known secret literal"
```

#### Windows-oriented tooling

Examples:

```powershell
llvm-strings.exe <artifact> | Select-String -SimpleMatch "known secret literal"
```

or, if needed for a deeper inspection:

```powershell
dumpbin /rawdata <artifact>
```

## Suggested negative-control test cases

When the implementation lands, include examples that intentionally protect:

- a short ASCII literal,
- a longer sentence-like literal, and
- at least one literal that is easy to recognize in a `strings` scan.

Also include one unprotected control literal in a test-only artifact so the
inspection toolchain itself is proven to work.

## Failure policy

Treat verification as **failed** if any of the following happens:

- a supposedly protected literal is visible in the Release artifact,
- the verification command cannot be reproduced from the repo docs,
- the result depends on a compiler-specific behavior that is undocumented, or
- the artifact only "passes" because the searched literal was optimized out
  entirely rather than being safely represented.

## Documentation policy

If the binary inspection reveals limitations, document them explicitly. Good
documentation for this MVP should answer:

- Which build types were checked?
- Which compilers/toolchains were checked?
- Which inspection command was used?
- What literal(s) were searched for?
- What caveat remains, if any?

That evidence should live alongside the examples/tests so later refactors can
re-run the same inspection workflow instead of inventing a new one from memory.
