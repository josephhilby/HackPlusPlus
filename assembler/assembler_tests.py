#!/usr/bin/env python3
from pathlib import Path
import subprocess
import os
import sys
import difflib

dir = os.path.dirname(os.path.abspath(sys.argv[0]))

ASM_DIR  = Path(dir + "/test/asm")
DIST_DIR = Path(dir + "/test/dest")
GOLD_DIR = Path(dir + "/test/hack")

def read_text(p: Path) -> str:
    # Read as text, preserve exact newlines for comparison
    return p.read_text(encoding="utf-8", newline="")

def run_one(main_exe: Path, asm_path: Path) -> bool:
    out_path  = DIST_DIR / (asm_path.stem + ".hack")
    gold_path = GOLD_DIR / (asm_path.stem + ".hack")

    if not gold_path.exists():
        print(f"[SKIP] {asm_path.name}: missing expected {gold_path}")
        return True

    proc = subprocess.run(
        [str(main_exe), str(asm_path), str(out_path)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )

    if proc.returncode != 0:
        print(f"[FAIL] {asm_path.name}: assembler exited {proc.returncode}")
        if proc.stderr.strip():
            print(proc.stderr.rstrip("\n"))
        return False

    got = read_text(out_path)
    exp = read_text(gold_path)

    if got == exp:
        print(f"[PASS] {asm_path.name}")
        return True

    got_lines = got.splitlines(True)  # keep line endings
    exp_lines = exp.splitlines(True)

    # Find first differing line
    max_len = max(len(got_lines), len(exp_lines))
    first = None
    for i in range(max_len):
        g = got_lines[i] if i < len(got_lines) else ""
        e = exp_lines[i] if i < len(exp_lines) else ""
        if g != e:
            first = i
            break

    line_no = (first + 1) if first is not None else "?"
    print(f"[FAIL] {asm_path.name}: differs at line {line_no}")

    if first is not None:
        g = got_lines[first] if first < len(got_lines) else "<EOF>"
        e = exp_lines[first] if first < len(exp_lines) else "<EOF>"
        print(f"  got:      {g!r}")
        print(f"  expected: {e!r}")

    # Show a small unified diff (first ~30 lines of diff output)
    diff = difflib.unified_diff(
        exp_lines, got_lines,
        fromfile=str(gold_path),
        tofile=str(out_path),
        lineterm=""
    )
    print("  diff:")
    for k, line in enumerate(diff):
        print("  " + line)
        if k >= 30:
            print("  ...")
            break

    return False

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 run_tests.py ./dest/main")
        sys.exit(2)

    main_exe = Path(sys.argv[1]).expanduser().resolve()
    if not main_exe.exists():
        print(f"Error: {main_exe} not found")
        sys.exit(2)

    if not ASM_DIR.exists():
        print(f"Error: {ASM_DIR} not found")
        sys.exit(2)

    DIST_DIR.mkdir(parents=True, exist_ok=True)

    asm_files = sorted(ASM_DIR.glob("*.asm"))
    if not asm_files:
        print(f"No .asm files found in {ASM_DIR}")
        sys.exit(0)

    all_ok = True
    for asm in asm_files:
        ok = run_one(main_exe, asm)
        all_ok = all_ok and ok

    sys.exit(0 if all_ok else 1)

if __name__ == "__main__":
    main()