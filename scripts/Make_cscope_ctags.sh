set -x
basedir=/f/Projects/Vaf/PEMx
subdir=IVY_Develop
srcdir=$basedir/$subdir
webdir=$srcdir/Vaf.Pemx.Web
project=$subdir

cscopeFiles=$project.cscope.files
cscopeFilesForCtags=$project.cscope_ctags.files
cscopeDb=$project.cscope.out
webFiles=$project.web.files
tempFiles=$$_tempfiles
batfile=${project}_GenCodeQueryDb.bat
shfile=${project}_GenCodeQueryDb.sh
codeQueryProject=$project-CodeQuery.db
webctagoptionsFile=.ctags_web

currdir=$PWD
cd $srcdir
srcdir=$(pwd)
cd $currdir

mkdbexe=cqmakedb

./Generate_files.sh $basedir $subdir

genScripts()
{
    echo "Generating scripts ..." >&2
    echo "" > $batfile
    echo "" > $shfile
    
    for line in "" \
                "set -x" \
                "cscope -cbR -f $cscopeDb -i $cscopeFiles" \
                "" \
                "ctags --fields=+i -n -R -L $cscopeFilesForCtags" \
                "" \
                "mv tags $project.tags.cs" \
                "" \
                "$mkdbexe -s $project.db -c $cscopeDb -t $project.tags.cs -p -d" \
                "" \
                "" \
                "ctags --options=\"$webctagoptionsFile\" --fields=+i -n -R -L $webFiles" \
                "" \
                "mv tags $project.tags.web" \
                "" \
                "$mkdbexe -s $project.db -t $project.tags.web -p -d" \
                "" \
                "codequery $project.db" \
                "" 
    do
        echo $line >> $batfile 
        echo $line >> $shfile 
    done
}

genLoop()
{
    fname=loop.sh
    fname_sh=$1
    rm -rf $fname
    touch $fname
    echo ". ~/.ksh_env_personal" >> $fname
    echo "" >> $fname
    echo "setx $project_CodeQuery" >> $fname
    echo "" >> $fname
    echo "while true" >> $fname
    echo "do" >> $fname
    echo "    ./$fname_sh" >> $fname
    echo "done" >> $fname
    chmod +x $fname
}

genScripts
genLoop $shfile

# for use in other script genall.sh
echo "$basedir $subdir"
