#!/bin/sh
CLANG_ROOT=/opt/homebrew/Cellar/llvm/13.0.1_1/
for f in $(find wipesrc -name '*.cpp' -or -name '*.h'); do
    $CLANG_ROOT/bin/clang-format -i "$f"
done
