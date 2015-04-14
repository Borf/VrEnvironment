start "Sync Left" robocopy /E /PURGE /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime" w:\runtime
start "Sync Front" robocopy /E /PURGE /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime" x:\runtime
start "Sync Right" robocopy /E /PURGE /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime" y:\runtime
start "Sync Bottom" robocopy /E /PURGE /V /NP /ZB /R:2 /W:1 "%CAVE%\runtime" z:\runtime
