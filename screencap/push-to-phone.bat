adb wait-for-device
adb remount

adb push ..\..\..\out\target\product\generic\system\bin\scap /system/bin/
adb shell chmod 777 /system/bin/scap

@pause
