#!/bin/bash

if [ -d build.release ]
then
	rm -r build.release
fi
meson setup --buildtype=release -Db_sanitize=none build.release
ln -s ../.make_install_release.sh build.release/make_install_release.sh
#meson setup build.fast
