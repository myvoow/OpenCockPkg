#!/bin/bash
CONFIGURATION=DEBUG

source edksetup.sh
build -a X64 -p OpenCockPkg/OpenCockPkg.dsc -t XCODE5 -b $CONFIGURATION
