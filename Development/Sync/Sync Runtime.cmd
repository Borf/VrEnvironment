start "Sync Left" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/runtime l:\runtime\ /XD c:\vrcave\runtime\data\models\LoL /XD c:\vrcave\.svn
start "Sync Front" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/runtime m:\runtime\ /XD c:\vrcave\runtime\data\models\LoL /XD c:\vrcave\.svn
start "Sync Right" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/runtime n:\runtime\ /XD c:\vrcave\runtime\data\models\LoL /XD c:\vrcave\.svn
start "Sync Bottom" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/runtime o:\runtime\ /XD c:\vrcave\runtime\data\models\LoL /XD c:\vrcave\.svn

REM start "Sync Left" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/runtime l:\runtime\
REM start "Sync Front" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/runtime m:\runtime\
REM start "Sync Right" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/runtime n:\runtime\ 
REM start "Sync Bottom" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/runtime o:\runtime\
