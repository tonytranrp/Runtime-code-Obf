#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path
import sys


def contains_plaintext(binary: bytes, needle: str) -> list[str]:
    encodings = {
        "utf-8": needle.encode("utf-8"),
        "utf-16le": needle.encode("utf-16le"),
    }
    return [name for name, encoded in encodings.items() if encoded in binary]


def main() -> int:
    parser = argparse.ArgumentParser(description="Reject binaries that still contain plaintext secrets.")
    parser.add_argument("binary", type=Path, help="Path to the compiled binary to inspect")
    parser.add_argument("needles", nargs="+", help="Plaintext strings that must not appear in the binary")
    args = parser.parse_args()

    binary = args.binary.read_bytes()
    failures: list[str] = []
    for needle in args.needles:
      matches = contains_plaintext(binary, needle)
      if matches:
          failures.append(f"{needle!r} found as {', '.join(matches)}")

    if failures:
        print(f"FAIL: plaintext leakage detected in {args.binary}")
        for failure in failures:
            print(f"  - {failure}")
        return 1

    print(f"PASS: no plaintext needles found in {args.binary}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
