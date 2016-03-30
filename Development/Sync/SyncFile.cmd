REM if NOT "%COMPUTERNAME%"=="PC121010" goto END
cd %~dp0\..\..\runtime
copy /y "%*" "w:\runtime\%*"
copy /y "%*" "x:\runtime\%*"
copy /y "%*" "y:\runtime\%*"
copy /y "%*" "z:\runtime\%*"
:END