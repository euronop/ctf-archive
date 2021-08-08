#!/bin/sh

if [ $# -ne 1 ]; then
    echo "Usage: $0 file"
    exit 1
fi

objdump -j .text -d "$1" | grep -E "c3.*ret" | sed -e 's/ *\([0-9a-f]*\).*/\1/' > "$1".patchpoints

n=`cat "$1".patchpoints | wc -l`

echo "Generated $n patchpoints."
