#!/bin/sh



RFCDIR=$1/rfc


find ${RFCDIR} -name "*.txt" -exec asciidoc -v --doctype book -b xhtml11 -a toc -a toclevels=3 -a numbered '{}' \;
find ${RFCDIR} -name "*.html" -exec dos2unix '{}' \;

