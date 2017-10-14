#!/bin/bash

# NAME
#
#   ctl - Control project's building
#
# SYNOPSIS
#
#   ctl build   build this project
#   ctl clear   clear build files
# 
# AUTHORS
#
#   Kevin Leptons <kevin.leptons@gmail.com>

# bash options
set -e

NAME="glee"
VERSION="0.1.0"
ROOT=$(realpath .)
INC="$ROOT/include"
SRC="$ROOT/src"
DEST="$ROOT/dest"
DEST_MAN="$DEST/man/$NAME.3.gz"
DIST="$ROOT/dist"
DIST_PKG="$DIST/$NAME"

# help menu
HELP="USAGE\n
    $(basename $0) build [--release]        build repo
    $(basename $0) clear                    clear build files
    $(basename $0) dist                     pack repo into .deb file
    $(basename $0) -h                       show help menu\n"

ctl_build()
{
    # prepare
    mkdir -vp $DEST
    cd $DEST

    local BUILD_FLAGS="-DCMAKE_BUILD_TYPE=Debug"
    for opt in "$@"; do
        if [ "$opt" = "--release" ]; then
            BUILD_FLAGS="-DCMAKE_BUILD_TYPE=Release"
        fi
    done
    echo $BUILD_FLAGS

    # use cmake, make to build
    cmake "$BUILD_FLAGS" ..
    make

    # man page
    mkdir -vp $(dirname $DEST_MAN)
    cp $ROOT/$NAME.3.man $DEST_MAN
    sed -i "s/\$BUILD_DATE/$(date)/g" $DEST_MAN
    sed -i "s/\$VERSION/$VERSION/g" $DEST_MAN
}

ctl_dist()
{
    ctl_clear
    ctl_build --release

    cd $ROOT
    rm -rf $DIST 
    mkdir -vp $DIST_PKG
    cp -r debian $DIST_PKG/DEBIAN
    sed -i "s/\$NAME/$NAME/g" $DIST_PKG/DEBIAN/control
    sed -i "s/\$VERSION/$VERSION/g" $DIST_PKG/DEBIAN/control

    cd $DEST
    make install DESTDIR=$DIST_PKG

    mkdir -vp $DIST_PKG/usr/share/man/man3
    cp $DEST_MAN $DIST_PKG/usr/share/man/man3

    local DIST_DOC="$DIST_PKG/usr/share/doc/$NAME"
    mkdir -vp $DIST_DOC
    cp $ROOT/doc/*.txt $DIST_DOC

    cd $DIST
    dpkg-deb --build -D $DIST_PKG $DIST
}

ctl_clear()
{
    rm -rf $DEST $DIST
    rm -rf cmake-build-debug
    if [ -f core ]; then
        rm core
    fi
}

ctl_help()
{
    printf "$HELP"
}

# parse arguments
case "$1" in
    build) ctl_build "${@:2}"; exit 0;;
    dist) ctl_dist; exit 0;;
    clear) ctl_clear; exit 0;;
    -h) ctl_help; exit 0;;
    *) ctl_help; exit 1;;
esac
