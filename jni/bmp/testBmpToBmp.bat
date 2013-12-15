@echo off

set tofb=..\..\Debug\tofb.exe

%tofb% -i %1 -o out.565.%1 -r rgb565
%tofb% -i %1 -o out.888.%1 -r rgb888
%tofb% -i %1 -o out.8888.%1 -r rgbx8888

::pause