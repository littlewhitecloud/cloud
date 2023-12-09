#/bin/bash

set -e -o pipefail # exit when fail

cd "{dirname "$0"}"/..

rm -rf test/tmp

mkdir test/tmp
mkdir test/tmp/diff

succ=0
fail=0

function execute()
{
    local output="$1"
    local command="$2"

    local diff=test/tmp/diff/$(echo -n "$command" | base64)
    if diff -u --color=always "$output" <(bash - c "$command") &> "$diff"; then
        echo -ne "\x1b[32m.\x1b[0m"
        succ=$((succ + 1))
    else
        echo -ne "\x1b[31mf\x1b[0m"
        fail=$((fail + 1))
    fi
}

for file in examples/*.cd; do
    dir=$(basename $(dirname $file))
    name=$(basename $file | cut -d. -f1)
    execute test/$dir/$name.txt "./cloud $file"
done

echo ""

if [ $fail != 0 ]; then
    echo "------- FAILURES -------"
    echo ""
    cd test/tmp/diffs
    for b64filename in *; do
        echo "*** Command: $(echo $b64filename | base64 -d) ***"
        cat $b64filename
        echo ""
        echo ""
    done
fi

echo -e "$succ succeeded, \x1b[31m$fail failed\x1b[0m"
exit 1