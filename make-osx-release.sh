#!/bin/bash

make clean
CFLAGS=-O3 make all

APP="NikwiDeluxe.app"

rm -fr $TGT $APP "Nikwi Deluxe.app" dmgfolder

rm -fr $APP
mkdir $APP
mkdir $APP/Contents
mkdir $APP/Contents/MacOS
mkdir $APP/Contents/Resources
cp infoplist.txt $APP/Contents/Info.plist
cp osxicon.icns $APP/Contents/Resources/nikwi.icns
echo 'APPL????' > $APP/Contents/PkgInfo
cp nikwi $APP/Contents/MacOS 
cp justdata.up $APP/Contents/Resources
strip $APP/Contents/MacOS/nikwi
mkdir dmgfolder
mv $APP "dmgfolder/Nikwi Deluxe.app"
hdiutil create NikwiDeluxe.dmg -srcfolder dmgfolder -ov -volname "Nikwi Deluxe" -format UDBZ

