adb shell mkdir /mnt/sdcard/bmp/

goto doaction

::push file to devices
adb push in.565.n.893x362.bmp  /mnt/sdcard/bmp
adb push in.565.p.893x362.bmp  /mnt/sdcard/bmp
adb push in.888.500x747.bmp    /mnt/sdcard/bmp
adb push in.888.n.893x362.bmp  /mnt/sdcard/bmp
adb push in.888.p.893x362.bmp  /mnt/sdcard/bmp
adb push in.8888.n.893x362.bmp /mnt/sdcard/bmp
adb push in.8888.p.893x362.bmp /mnt/sdcard/bmp

:doaction

adb push ../../libs/armeabi/tofb /system/bin
adb shell chmod 777 /system/bin/*

adb shell cd /mnt/sdcard/bmp

adb shell tofb -o fb -i /mnt/sdcard/bmp/in.565.n.893x362.bmp
@ping 127.0.0.1 -n 2 >nul
adb shell tofb -o fb -i /mnt/sdcard/bmp/in.888.500x747.bmp
@ping 127.0.0.1 -n 2 >nul

pause