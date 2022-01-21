#!/bin/bash -eux

PATH_FILE=${GITHUB_PATH:-.path}
TARGET_OS=${PDFium_TARGET_OS:?}
TARGET_CPU=${PDFium_TARGET_CPU:?}
CURRENT_CPU=${PDFium_CURRENT_CPU:-x64}

DepotTools_URL='https://chromium.googlesource.com/chromium/tools/depot_tools.git'
DepotTools_DIR="$PWD/depot_tools"
WindowsSDK_DIR="/c/Program Files (x86)/Windows Kits/10/bin/10.0.19041.0"

# Download depot_tools if not exists in this location
if [ ! -d "$DepotTools_DIR" ]; then
  git clone "$DepotTools_URL" "$DepotTools_DIR"
fi

echo "$DepotTools_DIR" >> "$PATH_FILE"

case "$TARGET_OS-$TARGET_CPU" in
  win-*)
    echo "$WindowsSDK_DIR/$CURRENT_CPU" >> "$PATH_FILE"
    ;;

  linux-arm)
    sudo apt-get update
    sudo apt-get install -y g++-arm-linux-gnueabihf
    ;;

  linux-arm64)
    sudo apt-get update
    sudo apt-get install -y g++-aarch64-linux-gnu
    ;;
esac