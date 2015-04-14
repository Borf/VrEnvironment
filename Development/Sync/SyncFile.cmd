if NOT "%COMPUTERNAME%"=="PC121010" goto END
cd %~dp0\..\..\runtime
copy /y "%*" "l:\runtime\%*"
copy /y "%*" "m:\runtime\%*"
copy /y "%*" "n:\runtime\%*"
copy /y "%*" "o:\runtime\%*"
:END