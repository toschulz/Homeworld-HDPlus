#!/bin/bash

if [ -f homeworld ]
then
	rm homeworld
fi

meson compile

if [ -f homeworld ]
then
        cp homeworld ../../homeworld.hdplus
fi

