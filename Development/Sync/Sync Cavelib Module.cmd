IF "LOCAL"=="%DEVMODE%" GOTO LOCAL

copy /y "%CAVE%runtime\libraries\cavelibplugins\%1" w:\runtime\libraries\cavelibplugins\
copy /y "%CAVE%runtime\libraries\cavelibplugins\%1" x:\runtime\libraries\cavelibplugins\
copy /y "%CAVE%runtime\libraries\cavelibplugins\%1" y:\runtime\libraries\cavelibplugins\
copy /y "%CAVE%runtime\libraries\cavelibplugins\%1" z:\runtime\libraries\cavelibplugins\

goto END

:LOCAL
echo Local development, not pushing to servers
:END