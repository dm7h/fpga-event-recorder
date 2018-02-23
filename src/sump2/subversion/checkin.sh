#!/bin/csh
svn diff   ../src
svn status ../src
svn commit ../src     -m $1
svn update ../src
svn commit ../xilinx  -m $1
svn update ../xilinx
