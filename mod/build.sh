#!/bin/sh

# build Cache Cleaner
cd jni
ndk-build

# make build dir
cd ../
rm -rf build
cp -rf source build

# copy Cache Cleaner
mkdir -p build/bin
cp -rf libs/arm64-v8a build/bin/arm64
cp -rf libs/armeabi-v7a build/bin/arm
cp -rf libs/x86_64 build/bin/x86_64
cp -rf libs/x86 build/bin/x86

# process file
cp -rf encrypt/cmd.sh encrypt.sh
chmod 0777 busybox
./busybox dos2unix encrypt.sh verify.sh changeTime.sh
./busybox sh encrypt.sh overfiles 3 build
sh verify.sh
cp -rf ../README.md build/README.md
cp -rf changelog.md build/changelog.md
sh changeTime.sh

# pack file
cd build
zip -r -9 CacheCleaner-Mod.zip .
mv CacheCleaner-Mod.zip ../

# exit
exit 0
