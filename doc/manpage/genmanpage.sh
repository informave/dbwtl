#!/bin/sh



MANUALDIR=$1/manpage

COM='//' ../../pipadoc  `find ../../ -type f -regex ".*\(hh\|cc\)"` ${MANUALDIR}/*.txt ${MANUALDIR}/manpage.conf > dbwtl.3.txt
a2x -v -f manpage dbwtl.3.txt

