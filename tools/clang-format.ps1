#!/usr/bin/env pwsh
#
# Run clang-format on all source files.
#
# Usage:
#
#     ./tools/clang-format.ps1 -i
#
#     ./tools/clang-format.ps1 --dry-run -Werror
#

$files = Get-ChildItem -Directory |
    Where-Object Name -notmatch '^(\.|bazel-|third_party)' |
    ForEach-Object { Get-ChildItem -Path $_.FullName -Recurse -Include *.cpp, *.hpp } |
    Select-Object -ExpandProperty FullName

clang-format --style=file --fallback-style=none @args @files
