#!/bin/bash

# --------------------------------------------------------------------------
# -- A premake the master script for the premake files
# --------------------------------------------------------------------------
set -x

projects=(
        "WheelChair"
        "GPIO_Utility"
        )

export CXX='gcc-4.9'
export CC='gcc-4.9'

ARGS='--cc=gcc'

for proj in ${projects[@]}; 
do
        premake4 $ARGS --file='./WheelChair/'${proj}'/premake4.lua' gmake
        make --file='./WheelChair/'${proj}'/Makefile'
done

