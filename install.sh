#!/bin/sh

cd daemon
echo "\n=== Building Executeables ===\n"
echo "homecontroldaemon..."
g++ -o homecontroldaemon homecontroldaemon.cpp -lrt
echo "homecontrol..."
g++ -o homecontrol homecontrol.cpp -lrt

echo "\n=== Installing Executeables ===\n"
service homecontroldaemon stop
cp -v homecontroldaemon /usr/sbin
service homecontroldaemon start
cp -v homecontrol /usr/bin
cd ..

echo "\n=== Installing Initscript ===\n"
cp -v ./daemon/homecontroldaemon.init.d /etc/init.d/homecontroldaemon
update-rc.d homecontroldaemon defaults
 
echo "\n=== Installing udev rules file ===\n"
cp -v ./daemon/99-homecontrol.rules /etc/udev/rules.d/


echo "\n=== Installing Webpages ===\n"
rm -rf /var/www/homecontrol
cp -vR ./www /var/www/homecontrol
echo "\n=== Finished, homecontrol installed in /usr/bin, type homecontrol at prompt to use it or goto http://<serverip>/homecontrol  ==="
