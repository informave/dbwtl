#!/bin/sh



GUIDEDIR=$1/guide

awk '{ print "//APIREF.050 "$0; }' ${GUIDEDIR}/apiref.txt > ./apiref.txx;
COM='//' ../../pipadoc  `find ../../ -type f -regex ".*\(hh\|cc\)"` ${GUIDEDIR}/*.txt  ./*.txx ${GUIDEDIR}/guide.conf > guide.txt
#a2x -v -f xhtml guide.txt
asciidoc -v --doctype book -a toc -a toclevels=3 -a numbered guide.txt

