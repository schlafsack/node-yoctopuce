#!/bin/sh
yapi_directory="yapi"
yapi_archive="YoctoLib.cpp.10105.zip"
yapi_url="http://www.yoctopuce.com/EN/downloads/"
yapi_excludes=

#yapi_source_folder="Sources/yapi"

echo "Getting yapi...";

if [ ! -d $yapi_directory ]; then
	echo "Creating working directory..."
	mkdir $yapi_directory
fi

cd $yapi_directory
echo "Working directory is "$(pwd)

if [ ! -f $yapi_archive ]; then
	echo "Downloading archive..."
	wget $yapi_url$yapi_archive
fi	

echo "Extracting archive..."	
unzip -qq -u -j $yapi_archive Sources/yapi* -d src
echo "Done getting yapi."

rm $yapi_archive
echo "Clean up complete."
