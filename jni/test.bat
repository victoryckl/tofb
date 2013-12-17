adb wait-for-device
adb remount
adb push ../libs/armeabi/tofb /system/bin
adb shell chmod 777 /system/bin/*
@pause