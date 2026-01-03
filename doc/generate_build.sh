set -x

builddir="build-Debug"
srcdir="../CodeQueryQt6"
if [ ! -d "$builddir" ]; then
    mkdir $builddir
fi

cd $builddir

cmake -S $srcdir -B . \
  -G "MinGW Makefiles" \
  -DCMAKE_PREFIX_PATH="/d/Qt/Tools/mingw1120_64" \
  -DCMAKE_BUILD_TYPE=Debug

