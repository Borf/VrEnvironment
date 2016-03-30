SET P=%~DP0
SET P=%P%..\..\

echo %P%

start "Sync Left" robocopy /E /PURGE /NP /NDL /R:2 /W:1 %P%Runtime X:\Runtime /XD %P%.git
start "Sync Front" robocopy /E /PURGE /NP /NDL /R:2 /W:1 %P%Runtime Z:\Runtime /XD %P%.git
start "Sync Right" robocopy /E /PURGE /NDL /NP /R:2 /W:1 %P%Runtime W:\Runtime /XD %P%.git
start "Sync Bottom" robocopy /E /PURGE /NDL /NP /R:2 /W:1 %P%Runtime Y:\Runtime /XD %P%.git
