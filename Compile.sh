#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
  # macOS
  CPU_COUNT=$(sysctl -n hw.logicalcpu)
else
  # Linux & other
  CPU_COUNT=$(nproc)
fi

make -j $(( CPU_COUNT + 1 )) all
