SET P=%~DP0
SET P=%P%..\..\

echo %P%

start "Sync Left" robocopy /E /PURGE /NP /NDL /R:2 /W:1 %P%Development X:\Development /XD %P%.git
start "Sync Front" robocopy /E /PURGE /NP /NDL /R:2 /W:1 %P%Development Z:\Development /XD %P%.git
start "Sync Right" robocopy /E /PURGE /NDL /NP /R:2 /W:1 %P%Development W:\Development /XD %P%.git
start "Sync Bottom" robocopy /E /PURGE /NDL /NP /R:2 /W:1 %P%Development Y:\Development /XD %P%.git
