FROM emscripten/emsdk:5.0.7 AS builder

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential pkg-config gettext bison flex \
        ninja-build python3 python3-pip \
        autoconf automake libtool autotools-dev \
        xz-utils bzip2 ca-certificates \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN pip3 install --break-system-packages --no-cache-dir meson==1.7.0 \
 && meson --version

ARG VALA_VERSION=0.56.19
ENV VALA_VERSION=${VALA_VERSION}
ARG VALA_SHA256=5ad7cbbfcc0de61b403d6797c9ef60455bfbebd8e162aec33b5b0b097adfb9d5

ARG GLIB_VERSION=2.88.1
ARG GLIB_SHA256=51ab804c56f6eab3e5045c774d1290ac5e4c923d4f9a3d8e33123bee45c1840e

ARG PCRE2_VERSION=10.47
ARG PCRE2_SHA256=47fe8c99461250d42f89e6e8fdaeba9da057855d06eb7fc08d9ca03fd08d7bc7

ARG LIBFFI_VERSION=3.5.2
ARG LIBFFI_SHA256=f3a3082a23b37c293a4fcd1053147b371f2ff91fa7ea1b2a52e335676bac82dc

ENV OUT_DIR=/out
ENV ROOT=/src
WORKDIR /src
RUN mkdir -p $OUT_DIR

ENV CFLAGS="-O2 -fPIC -pthread -Wno-error=incompatible-function-pointer-types -Wno-error=incompatible-pointer-types"
ENV CXXFLAGS="${CFLAGS}"

ADD https://github.com/libffi/libffi/releases/download/v${LIBFFI_VERSION}/libffi-${LIBFFI_VERSION}.tar.gz /src/libffi.tar.gz
RUN echo "${LIBFFI_SHA256}  libffi.tar.gz" | sha256sum -c - \
 && tar -xzf libffi.tar.gz && rm libffi.tar.gz \
 && cd libffi-${LIBFFI_VERSION} \
 && emconfigure ./configure \
        --host=wasm32-unknown-emscripten \
        --prefix=$OUT_DIR \
        --disable-shared --enable-static \
        --disable-docs \
        --disable-multi-os-directory \
        --disable-exec-static-tramp \
 && emmake make -j$(nproc) \
 && emmake make install

ADD https://github.com/PCRE2Project/pcre2/releases/download/pcre2-${PCRE2_VERSION}/pcre2-${PCRE2_VERSION}.tar.bz2 /src/pcre2.tar.bz2
RUN echo "${PCRE2_SHA256}  pcre2.tar.bz2" | sha256sum -c - \
 && tar -xjf pcre2.tar.bz2 && rm pcre2.tar.bz2 \
 && cd pcre2-${PCRE2_VERSION} \
 && emconfigure ./configure \
        --host=wasm32-unknown-emscripten \
        --prefix=$OUT_DIR \
        --disable-shared --enable-static \
        --enable-pcre2-8 --disable-pcre2-16 --disable-pcre2-32 \
        --disable-jit \
        --disable-pcre2grep-libz --disable-pcre2grep-libbz2 \
 && emmake make -j$(nproc) \
 && emmake make install

COPY deps /src/deps

RUN emcc -O2 -fPIC -pthread -c /src/deps/wasm-stubs.c -o /src/wasm-stubs.o \
 && ls -l /src/wasm-stubs.o

ADD https://download.gnome.org/sources/glib/2.88/glib-${GLIB_VERSION}.tar.xz /src/glib.tar.xz
RUN echo "${GLIB_SHA256}  glib.tar.xz" | sha256sum -c - \
 && tar -xJf glib.tar.xz && rm glib.tar.xz \
 && cd glib-${GLIB_VERSION} \
 && sed -i "/^network_args = \[ \]\$/{n;s/^if host_system != 'windows'\$/if host_system != 'windows' and host_system != 'emscripten'/}" gio/meson.build \
 && grep -q "host_system != 'emscripten'" gio/meson.build \
 && sed -i "s|#error \"g_unix_fd_query_path() not supported on this platform\"|g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_NOSYS, \"g_unix_fd_query_path() not supported on this platform\"); return NULL;|" glib/glib-unix.c \
 && grep -q 'G_FILE_ERROR_NOSYS, "g_unix_fd_query_path' glib/glib-unix.c \
 && PKG_CONFIG_LIBDIR=$OUT_DIR/lib/pkgconfig \
    meson setup _build \
        --cross-file /src/deps/emscripten.cross \
        --prefix=$OUT_DIR \
        --buildtype=release \
        --default-library=static \
        -Dtests=false \
        -Ddocumentation=false \
        -Dman-pages=disabled \
        -Dintrospection=disabled \
        -Dnls=disabled \
        -Dxattr=false \
        -Dselinux=disabled \
        -Dlibmount=disabled \
        -Ddtrace=disabled \
        -Dsystemtap=disabled \
        -Dglib_debug=disabled \
        -Dglib_assert=false \
        -Dglib_checks=false \
        -Dlibelf=disabled \
        -Doss_fuzz=disabled \
        -Dinstalled_tests=false \
 && meson compile -C _build \
 && meson install -C _build \
 && ls $OUT_DIR/lib/pkgconfig

ADD https://download.gnome.org/sources/vala/0.56/vala-${VALA_VERSION}.tar.xz /src/vala.tar.xz
RUN echo "${VALA_SHA256}  vala.tar.xz" | sha256sum -c - \
 && tar -xJf vala.tar.xz && rm vala.tar.xz

COPY js /src/js

ENV VALA_LDFLAGS="\
    -lnodefs.js -lworkerfs.js \
    --post-js /src/js/post.js \
    -pthread \
    -sPTHREAD_POOL_SIZE=4 \
    -sPTHREAD_POOL_SIZE_STRICT=0 \
    -sWASM_BIGINT=1 \
    -sALLOW_MEMORY_GROWTH=1 \
    -sALLOW_TABLE_GROWTH=1 \
    -sINITIAL_MEMORY=134217728 \
    -sSTACK_SIZE=5242880 \
    -sEXIT_RUNTIME=0 \
    -sEXPORTED_RUNTIME_METHODS=callMain,FS,NODEFS,WORKERFS,ENV \
    -sINCOMING_MODULE_JS_API=noInitialRun,wasmBinary,locateFile,preRun,postRun,instantiateWasm,print,printErr,arguments,mainScriptUrlOrBlob \
    -sINVOKE_RUN=0 \
    -sMODULARIZE=1 \
    -sEXPORT_ES6=1 \
    -sERROR_ON_UNDEFINED_SYMBOLS=0 \
"

RUN cd vala-${VALA_VERSION} \
 && EM_PKG_CONFIG_PATH=$OUT_DIR/lib/pkgconfig \
    emconfigure ./configure \
        --host=wasm32-unknown-emscripten \
        --prefix=$OUT_DIR \
        --disable-shared --enable-static \
        --disable-valadoc \
        --disable-unversioned \
        GI_GIRDIR=$OUT_DIR/share/gir-1.0 \
        EXEEXT=.mjs \
 && EM_PKG_CONFIG_PATH=$OUT_DIR/lib/pkgconfig \
    emmake make -j$(nproc) \
        LDFLAGS="${VALA_LDFLAGS}" \
 && emmake make install

RUN cd vala-${VALA_VERSION}/compiler \
 && ls -lh valac* *.o \
 && test -f valacompiler.o \
 && test -f valac.wasm

RUN mkdir -p /sdk-pkg \
 && test -f $OUT_DIR/share/vala-0.56/vapi/glib-2.0.vapi \
 && du -sh $OUT_DIR/share/vala-0.56 \
 && python3 /emsdk/upstream/emscripten/tools/file_packager.py \
        /sdk-pkg/vala-vapi.data \
        --preload $OUT_DIR/share/vala-0.56@$OUT_DIR/share/vala-0.56 \
        --js-output=/sdk-pkg/vala-vapi-fs.js \
        --export-name=Module \
 && ls -lh /sdk-pkg

RUN cd vala-${VALA_VERSION}/compiler \
 && OBJ_FILES=$(ls *.o 2>/dev/null) \
 && LIBVALA_A=$(find /src/vala-${VALA_VERSION}/vala -name 'libvala-*.a' | head -1) \
 && LIBVALACC_A=$(find /src/vala-${VALA_VERSION}/ccode -name 'libvalaccode*.a' | head -1) \
 && LIBVALACG_A=$(find /src/vala-${VALA_VERSION}/codegen -name 'libvalaccodegen*.a' | head -1) \
 && ls -la "$LIBVALA_A" "$LIBVALACC_A" "$LIBVALACG_A" \
 && emcc \
      ${VALA_LDFLAGS} \
      --pre-js /sdk-pkg/vala-vapi-fs.js \
      -sFORCE_FILESYSTEM \
      -sEMULATE_FUNCTION_POINTER_CASTS=1 \
      -Wl,--wrap=g_spawn_command_line_sync \
      $OBJ_FILES \
      "$LIBVALACG_A" "$LIBVALACC_A" "$LIBVALA_A" \
      /src/wasm-stubs.o \
      $(PKG_CONFIG_LIBDIR=$OUT_DIR/lib/pkgconfig pkg-config --static --libs glib-2.0 gobject-2.0 gmodule-2.0) \
      -o vala.mjs \
 && ls -lh vala.mjs vala.wasm

RUN mkdir -p /dist \
 && cp /src/vala-${VALA_VERSION}/compiler/vala.mjs   /dist/vala.mjs \
 && cp /src/vala-${VALA_VERSION}/compiler/vala.wasm  /dist/vala.wasm \
 && cp /sdk-pkg/vala-vapi.data                       /dist/vala-vapi.data \
 && [ "$(head -c4 /dist/vala.wasm | od -An -tx1 | tr -d ' \n')" = "0061736d" ] \
 && for f in vala.mjs vala.wasm vala-vapi.data; do \
        [ "$(stat -c%s /dist/$f)" -gt 4096 ] || { echo "too small: $f" >&2; exit 1; }; \
    done \
 && ls -lh /dist

FROM scratch AS exportor
COPY --from=builder /dist /dist
