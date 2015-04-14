start "Sync Left" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/ l:\ /XD c:\vrcave\runtime\data\models\LoL /XD c:\vrcave\.svn
start "Sync Front" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/ m:\ /XD c:\vrcave\runtime\data\models\LoL /XD c:\vrcave\.svn
start "Sync Right" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/ n:\ /XD c:\vrcave\runtime\data\models\LoL /XD c:\vrcave\.svn
start "Sync Bottom" robocopy /E /PURGE /NP /R:2 /W:1 c:/vrcave/ o:\ /XD c:\vrcave\runtime\data\models\LoL /XD c:\vrcave\.svn
