# replace HEAD with 2 for Revision 2 etc
mkdir ./restored
svn checkout -r HEAD file://`pwd`/repos/src    ./restored/src
svn checkout -r HEAD file://`pwd`/repos/xilinx ./restored/xilinx
