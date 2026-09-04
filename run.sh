#!/bin/bash
set -e

if [[ ${1,,} == "release" ]]; then
    folder=build/linux/release
    shift
else
    folder=build/linux/debug
fi

if [[ ${1,,} == "refresh_generator" ]]; then
    rm -rf $folder/generator $folder/generator-prefix
    shift
fi
if [ ! -d $folder ]; then
    echo "Folder $folder doesn't exist. Run configure.sh first."
fi
cmake --build $folder -j4 $@
$folder/main/embdfs-main
