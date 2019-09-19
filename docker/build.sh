#!/bin/bash -x

./scripts/feeds update custom && \
  ./scripts/feeds install orc && \
  make package/orc/compile

mkdir -p /restconf/build
cp -a ./bin/packages/x86_64/custom/. /restconf/build