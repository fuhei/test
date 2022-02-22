#!/bin/bash
# rongtao@cestc.cn
# see also https://github.com/rtoax/libcareplus/

usage()
{
	echo "Usage: check_buildid <ELF file>"
	exit 1
}

if [ $# -lt 1 ]; then
	usage
fi

ELFFILE=$1

if [ ! -f $ELFFILE ]; then
	echo "$ELFFILE not exist."
	usage
fi

echo "$ELFFILE buildid is $(eu-readelf -n $ELFFILE | sed -n '/Build ID:/ { s/.* //; p }')"
