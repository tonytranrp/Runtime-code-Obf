#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path
import re
import sys


def contains_plaintext(binary: bytes, needle: str) -> list[str]:
    encodings = {
        "utf-8": needle.encode("utf-8"),
        "utf-16le": needle.encode("utf-16le"),
    }
    matches = [name for name, encoded in encodings.items() if encoded in binary]

    printable_segments = [
        segment.decode("ascii", errors="ignore")
        for segment in re.findall(rb"[ -~]{4,}", binary)
    ]
    if any(needle in segment for segment in printable_segments):
        matches.append("printable-ascii-segment")

    return matches


def report_contains_plaintext(report: str, needle: str) -> bool:
    return needle in report


def main() -> int:
    parser = argparse.ArgumentParser(description="Reject binaries that still contain plaintext secrets.")
    parser.add_argument("binary", type=Path, help="Path to the compiled binary to inspect")
    parser.add_argument(
        "--text-report",
        action="append",
        default=[],
        type=Path,
        help="Optional text artifacts (disassembly, dumpbin output, etc.) that must also stay plaintext-free",
    )
    parser.add_argument("needles", nargs="+", help="Plaintext strings that must not appear in the binary")
    args = parser.parse_args()

    binary = args.binary.read_bytes()
    failures: list[str] = []
    for needle in args.needles:
        matches = contains_plaintext(binary, needle)
        if matches:
            failures.append(f"{needle!r} found as {', '.join(matches)}")

    for report_path in args.text_report:
        report_text = report_path.read_text(encoding="utf-8", errors="ignore")
        for needle in args.needles:
            if report_contains_plaintext(report_text, needle):
                failures.append(f"{needle!r} found in text report {report_path}")

    if failures:
        print(f"FAIL: plaintext leakage detected in {args.binary}")
        for failure in failures:
            print(f"  - {failure}")
        return 1

    print(f"PASS: no plaintext needles found in {args.binary}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
