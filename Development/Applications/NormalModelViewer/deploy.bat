rmdir /y /s modelviewer
mkdir ModelViewer
copy ..\..\..\Runtime\NormalModelViewer.exe ModelViewer\ModelViewer.exe
robocopy ..\..\..\Runtime ModelViewer *.dll /XF PhysX3*DEBUG_x86.dll /XF PhysX3CharacterKinematic_x86.dll /XF PhysX3Common_x86.dll /XF PhysX3Cooking_x86.dll /XF PhysX3Gpu_x86.dll
robocopy ..\..\..\Runtime\data\vrlib ModelViewer\data\vrlib /e /XD rendermodels
pause