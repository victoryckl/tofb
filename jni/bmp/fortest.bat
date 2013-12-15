@echo off
for /r . %%i in (in.*.bmp) do (
	call testBmpToBmp.bat %%~nxi
)
pause