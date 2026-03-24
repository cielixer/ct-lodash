#!/usr/bin/env bash
set -euo pipefail

FILES="${@:-$(find include/ctl -name '*.hpp')}"

if grep -rn 'namespace dop' $FILES; then
  echo "❌ Found 'namespace dop' in installable headers (should be 'namespace ctl')"
  exit 1
fi

echo "✅ No 'namespace dop' found in installable headers"
exit 0
