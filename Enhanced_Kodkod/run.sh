#!/bin/bash

currentdir="$(dirname "$(realpath $0)")"

specfile=$1
satfile=$2
symfile=$3

java -cp $currentdir/src:$currentdir/bin:$currentdir/lib/org.alloytools.alloy.dist.jar edu.mit.csail.sdg.alloy4whole.ExampleUsingTheCompiler $specfile $satfile $symfile


