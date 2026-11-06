#!/usr/bin/env bash
# Build Yellow's Pro Micro firmware from the frozen source tree in source/
# (NikitaB04/ELRSController at a4c7877 plus 0001-yellow-*.patch, kept whole so the
# build needs no network). Runs the CRSF reader's host test first. Flashes nothing.
#
# Usage (on the Mac): firmware/promicro/elrscontroller/build.sh   → firmware.hex here
set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT
cp -R "$here/source/." "$work/"

(cd "$work" && c++ -std=c++11 -Wall -Wextra -Isrc test/test_crsf_reader.cpp \
  src/utils/CrsfReader.cpp -o "$work/test_crsf" && "$work/test_crsf")

(cd "$work" && pio run)
cp "$work/.pio/build/leonardo/firmware.hex" "$here/firmware.hex"
shasum -a 256 "$here/firmware.hex"
