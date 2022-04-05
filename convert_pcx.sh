#!/bin/sh
find WIPEOUT -iname '*.pcx' -exec convert {} {}.png \;
find WIPEOUT -iname '*.dpq' -exec convert {} {}.png \;
