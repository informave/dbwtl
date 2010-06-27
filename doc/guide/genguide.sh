#!/bin/sh



GUIDEDIR=$1/guide

COM='//' ../../pipadoc  `find ../../ -type f -regex ".*\(hh\|cc\)"` ${GUIDEDIR}/*.txt ${GUIDEDIR}/guide.conf > guide.txt
#a2x -v -f xhtml guide.txt
asciidoc -v --doctype book -a toc -a toclevels=4 -a numbered guide.txt

