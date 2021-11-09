#!/bin/bash -eux

PATCHES="$PWD/patches"
CUSTOMPATCHES="$PWD/custom"
SOURCE="${PDFium_SOURCE_DIR:-pdfium}"
OS="${PDFium_TARGET_OS:?}"

pushd "${SOURCE}"

[ "$OS" != "android" ] && git apply -v "$PATCHES/shared_library.patch"
git apply -v "$PATCHES/public_headers.patch"

[ "${PDFium_V8:-}" == "enabled" ] && git apply -v "$PATCHES/v8_init.patch"

if [ "$OS" == "win" ]; then
  git apply -v "$PATCHES/widestring.patch"
  git -C build apply -v "$PATCHES/rc_compiler.patch"

  VERSION=${PDFium_VERSION:-0.0.0.0}
  YEAR=$(date +%Y)
  VERSION_CSV=${VERSION//./,}
  export YEAR VERSION VERSION_CSV
  envsubst < "$PATCHES/resources.rc" > "resources.rc"
fi

cp "$CUSTOMPATCHES/pdfiumviewer.cpp" "./fpdfsdk/"

git apply -v "$CUSTOMPATCHES/include_pdfiumviewer.patch"

popd
