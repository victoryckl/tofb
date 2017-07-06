@echo off

adb push ..\..\libs\armeabi\tofb /data/local/
adb shell chmod 777 /data/local/*

adb shell mkdir /mnt/sdcard/bmp

adb shell /data/local/tofb -i fb -o /mnt/sdcard/bmp/fb.rgb565.bmp -r rgb565
adb shell /data/local/tofb -i fb -o /mnt/sdcard/bmp/fb.rgb888.bmp -r rgb888
adb shell /data/local/tofb -i fb -o /mnt/sdcard/bmp/fb.rgbx8888.bmp -r rgbx8888

mkdir bmp
adb pull /mnt/sdcard/bmp/fb.rgb565.bmp bmp/
adb pull /mnt/sdcard/bmp/fb.rgb888.bmp bmp/
adb pull /mnt/sdcard/bmp/fb.rgbx8888.bmp bmp/

pause