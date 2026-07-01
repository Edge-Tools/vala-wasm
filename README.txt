vala-wasm — Corresponding Source mirror
==================================================

This repository publishes the Corresponding Source for the WebAssembly
build of vala (license: LGPL-2.1-only) used in edgetools.io.

Contents
  build/      our build recipe: Dockerfile + helper scripts/config/patches.
              Rebuild with:  docker build build/
  upstream/   the exact upstream source archive(s) the build fetched,
              byte-identical and sha256-verified (see below).

Upstream sources:
  libffi.tar.gz
    https://github.com/libffi/libffi/releases/download/v3.5.2/libffi-3.5.2.tar.gz
    sha256 f3a3082a23b37c293a4fcd1053147b371f2ff91fa7ea1b2a52e335676bac82dc
  pcre2.tar.bz2
    https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.47/pcre2-10.47.tar.bz2
    sha256 47fe8c99461250d42f89e6e8fdaeba9da057855d06eb7fc08d9ca03fd08d7bc7
  glib.tar.xz
    https://download.gnome.org/sources/glib/2.88/glib-2.88.1.tar.xz
    sha256 51ab804c56f6eab3e5045c774d1290ac5e4c923d4f9a3d8e33123bee45c1840e
  vala.tar.xz
    https://download.gnome.org/sources/vala/0.56/vala-0.56.19.tar.xz
    sha256 5ad7cbbfcc0de61b403d6797c9ef60455bfbebd8e162aec33b5b0b097adfb9d5
