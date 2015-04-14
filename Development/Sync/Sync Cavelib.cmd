IF "LOCAL"=="%DEVMODE%" GOTO LOCAL

REM start "Sync Left" robocopy /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime\libraries\cavelib.dll" w:\runtime\libraries\
REM start "Sync Front" robocopy /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime\libraries\cavelib.dll" x:\runtime\libraries\
REM start "Sync Right" robocopy /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime\libraries\cavelib.dll" y:\runtime\libraries\
REM start "Sync Bottom" robocopy /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime\libraries\cavelib.dll" z:\runtime\libraries\

copy /y "%CAVE%runtime\libraries\cavelib.dll" w:\runtime\libraries\
copy /y "%CAVE%runtime\libraries\cavelib.dll" x:\runtime\libraries\
copy /y "%CAVE%runtime\libraries\cavelib.dll" y:\runtime\libraries\
copy /y "%CAVE%runtime\libraries\cavelib.dll" z:\runtime\libraries\

goto END

:LOCAL
echo Local development, not pushing to servers
:END