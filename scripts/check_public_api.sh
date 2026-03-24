#!/usr/bin/env bash
set -euo pipefail

# Scan public-facing files for Boost tokens, excluding allowed files
# Exclude: describe.hpp (contains BOOST_DESCRIBE_STRUCT macro), and detail/ subdirectory

FILES="${@:-$(find README.md examples/ include/ctl -type f \( -name '*.md' -o -name '*.cpp' -o -name '*.hpp' \) 2>/dev/null | grep -v 'include/ctl/describe.hpp' | grep -v 'include/ctl/detail/' || true)}"

# Only proceed if FILES is not empty
if [ -z "$FILES" ]; then
  echo "✅ No public-facing files to check"
  exit 0
fi

if grep -r -n -E 'boost::|BOOST_' $FILES; then
  echo "❌ Found Boost tokens in public-facing files (should be hidden in detail/)"
  exit 1
fi

echo "✅ No Boost tokens found in public-facing files"
exit 0
