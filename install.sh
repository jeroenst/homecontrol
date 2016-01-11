#!/bin/sh

cd daemon
echo "\n=== Building Executeables ===\n"
echo "homecontroldaemon..."
g++ -o homecontroldaemon homecontroldaemon.cpp -lrt
echo "homecontrol..."
g++ -o homecontrol homecontrol.cpp -lrt

echo "\n=== Installing Executeables ===\n"
#sudo cp 433mhzdaemon /usr/sbin
#sudo cp 433mhztool /usr/bin
#sudo chmod +s /usr/bin/433mhztool
#sudo cp kaku /usr/bin
#sudo chmod +s /usr/bin/kaku
#sudo cp action /usr/bin
#sudo chmod +s /usr/bin/action
cd ..

echo "\n=== Installing Webpages ===\n"
rm -rf /var/www/homecontrol
cp -R ./www /var/www/homecontrol
echo "\n=== Finished, kaku&action installed in /usr/bin, type kaku or action at prompt to use it or goto http://<raspberrypip>/domotica  ==="
