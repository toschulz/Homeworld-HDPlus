#!/bin/bash

rm -r build.debug

#meson setup --buildtype=release -Db_sanitize=none build.fast
meson setup build.debug
