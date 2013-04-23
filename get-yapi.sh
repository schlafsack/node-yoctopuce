#!/bin/sh
yapi_directory="./yapi"
yapi_url="https://github.com/yoctopuce/yoctolib_cpp.git"

echo "Getting yapi...";

if [ ! -d $yapi_directory ]; then
	git clone ${yapi_url}  $yapi_directory
else
 	cd $yapi_directory
 	git pull
fi

echo "Done getting yapi."
