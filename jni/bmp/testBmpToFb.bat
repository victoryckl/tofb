adb shell mkdir /mnt/sdcard/bmp/

::goto doaction

::push file to devices
adb push rgb565.bmp         /mnt/sdcard/bmp
adb push rgb888.bmp         /mnt/sdcard/bmp
adb push rgbx8888.bmp       /mnt/sdcard/bmp
adb push in.565.n.893x362.bmp  /mnt/sdcard/bmp
adb push in.565.p.893x362.bmp  /mnt/sdcard/bmp
adb push in.888.500x747.bmp    /mnt/sdcard/bmp
adb push in.8888.p.893x362.bmp /mnt/sdcard/bmp

:doaction

adb push ..\..\libs\armeabi\tofb /data/local/
adb shell chmod 777 /data/local/*

adb shell /data/local/tofb -o fb -i /mnt/sdcard/bmp/rgb565.bmp
@ping 127.0.0.1 -n 3 >nul
adb shell /data/local/tofb -o fb -i /mnt/sdcard/bmp/rgb888.bmp
@ping 127.0.0.1 -n 3 >nul
adb shell /data/local/tofb -o fb -i /mnt/sdcard/bmp/rgbx8888.bmp
@ping 127.0.0.1 -n 3 >nul
adb shell /data/local/tofb -o fb -i /mnt/sdcard/bmp/in.565.n.893x362.bmp
@ping 127.0.0.1 -n 3 >nul
adb shell /data/local/tofb -o fb -i /mnt/sdcard/bmp/in.888.500x747.bmp
@ping 127.0.0.1 -n 3 >nul

pause