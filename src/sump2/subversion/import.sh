# Import directories into Subversion
svn import ../src    file://`pwd`/repos/src    -m 'import'
svn import ../xilinx file://`pwd`/repos/xilinx -m 'import'
# Move Original directories here for safe keeping
mkdir orgs
mv  ../src    ./orgs
mv  ../xilinx ./orgs
# Checkout the the Mainline src from Subversion into ./src
svn checkout -r HEAD file://`pwd`/repos/src    ../src
svn checkout -r HEAD file://`pwd`/repos/xilinx ../xilinx
# Now copy original files back in to preserve timestamps
/bin/cp -r -p ./orgs/src/*    ../src/
/bin/cp -r -p ./orgs/xilinx/* ../xilinx/
