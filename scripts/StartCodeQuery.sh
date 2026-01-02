
base_and_subdir=`Make_cscope_ctags.sh`

echo "base, subdir: $base_and_subdir"

loop.sh &

echo "waiting 2min ..."
sleep 120

while true
do
    ./Generate_files.sh $base_and_subdir
    echo "waiting 2min ..."
    sleep 120
done
