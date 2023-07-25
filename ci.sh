#!/bin/bash -eu

./download-pkg.sh bee https://github.com/bec-ca/bee/archive/refs/tags/v2.2.1.tar.gz
./download-pkg.sh command https://github.com/bec-ca/command/archive/refs/tags/v2.1.0.tar.gz

for file in yasf/*.cpp; do
  echo "Compiling $file..."
  clang++ -c $(cat compile_flags.txt) $file
done
