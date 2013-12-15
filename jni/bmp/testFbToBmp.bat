@echo off

adb push ../../libs/armeabi/tofb /system/bin
adb shell chmod 777 /system/bin/*

adb shell mkdir /mnt/sdcard/bmp

adb shell tofb -i fb -o /mnt/sdcard/bmp/fb.rgb565.bmp -r rgb565
adb shell tofb -i fb -o /mnt/sdcard/bmp/fb.rgb888.bmp -r rgb888
adb shell tofb -i fb -o /mnt/sdcard/bmp/fb.rgbx8888.bmp -r rgbx8888

adb pull /mnt/sdcard/bmp/fb.rgb565.bmp .
adb pull /mnt/sdcard/bmp/fb.rgb888.bmp .
adb pull /mnt/sdcard/bmp/fb.rgbx8888.bmp .

pause