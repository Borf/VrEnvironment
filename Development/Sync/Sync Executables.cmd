IF "LOCAL"=="%DEVMODE%" GOTO LOCAL

REM start "Sync Left" robocopy /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime\*.exe" w:\runtime
REM start "Sync Front" robocopy /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime\*.exe" x:\runtime
REM start "Sync Right" robocopy /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime\*.exe" y:\runtime
REM start "Sync Bottom" robocopy /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime\*.exe" z:\runtime

robocopy /V /NP /Z /R:2 /W:1 "%CAVE%runtime" w:\runtime
robocopy /V /NP /Z /R:2 /W:1 "%CAVE%runtime" x:\runtime
robocopy /V /NP /Z /R:2 /W:1 "%CAVE%runtime" y:\runtime
robocopy /V /NP /Z /R:2 /W:1 "%CAVE%runtime" z:\runtime

goto END

:LOCAL
echo Local development, not pushing to servers
:END