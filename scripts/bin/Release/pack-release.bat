
@echo off

REM Format today's date to DAY-MONTH-YEAR format
SET year=%date:~-4%
SET month=%date:~3,2%
IF "%month:~0,1%" == " " SET month=0%month:~1,1%
SET day=%date:~0,2%
IF "%day:~0,1%" == " " SET day=0%day:~1,1%

REM Pack today's build to archive
SET BUILD_DATE=%day%-%month%-%year%
SET ARCHIVE_NAME=TRAChecker_%BUILD_DATE%.zip

"C:\Program Files\7-Zip\7z.exe" a %ARCHIVE_NAME% TRAChecker.exe wxbase310u_vc_custom.dll wxmsw310u_core_vc_custom.dll wxmsw310u_stc_vc_custom.dll ru/application.po ru/application.mo
