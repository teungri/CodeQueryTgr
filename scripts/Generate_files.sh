
set -x
basedir=$1
subdir=$2
srcdir=$basedir/$subdir
webdir=$srcdir/Vaf.Pemx.Web
project=$subdir

cscopeFiles=$project.cscope.files
cscopeFilesForCtags=$project.cscope_ctags.files
webFiles=$project.web.files
tempFiles=$$_tempfiles

currdir=$PWD
cd $srcdir
srcdir=$(pwd)
cd $currdir

echo "Searching web files *.html, *.js in $srcdir ..." >&2
find $srcdir \( -path $srcdir/.git -o \
                -path $srcdir/.history -o \
                -path $srcdir/.vshistory -o \
                -path $srcdir/packages -o \
                -path $webdir/obj -o \
                -path $webdir/Content/Gentelella -o \
                -path $webdir/Scripts/Gentelella -o \
                -path $webdir/Scripts/lib \) -prune -o \
                \( -name "*.js" -o -name "*.html" -o -name "*.css" \) \
                -print > $webFiles

cat $webFiles | sed -e "s/\/f\//F:\\//g" |  sed -e "s/\//\\\/g" > $tempFiles
mv $tempFiles $webFiles

echo "Searching C# files in $srcdir ..." >&2
find $srcdir \( -path $srcdir/.git -o \
                -path $srcdir/.history -o \
                -path $srcdir/.vshistory -o \
                -path $srcdir/packages -o \
                -path $webdir/obj -o \
                -path $webdir/Content/Gentelella -o \
                -path $webdir/Scripts/Gentelella -o \
                -path $webdir/Scripts/lib \) -prune -o \
                \( -name "*.cs" -o -name "*.cs" -o -name "*.R" \) \
                -print | grep -v "\/obj\/" | grep -v "\/bin\/" | grep -v "\/.vshistory\/" | grep -v "\/.history\/"  > $cscopeFilesForCtags

cat $cscopeFilesForCtags | sed -e "s/\/f\//F:\\//g" | \
                   sed -e "s/\//\\\/g" > $tempFiles
mv $tempFiles $cscopeFilesForCtags

cat $cscopeFilesForCtags | sed -e "s/^/\"/g" | \
                   sed -e "s/$/\"/g" > $tempFiles
mv $tempFiles $cscopeFiles
