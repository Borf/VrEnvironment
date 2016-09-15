cd %~DP0
rmdir /q /s NetworkEngine
md NetworkEngine
cd NetworkEngine
xcopy ..\..\..\release\networkengine.exe
xcopy ..\..\..\release\networkengine.pdb
xcopy /s ..\data\*.* .\data\
xcopy ..\..\..\..\runtime\*.dll .
xcopy ..\*.dll .
xcopy /s ..\..\..\..\runtime\configs\*.* .\configs\
xcopy /s ..\..\..\..\runtime\data\vrlib\*.* .\data\vrlib\

echo NetworkEngine.exe --config configs/sim.json --config configs/vive-sim.json > sim.bat
echo NetworkEngine.exe --config configs/openvr.json > vive.bat

cd ..
"c:\program files\winrar\winrar.exe" a -r -df -afzip NetworkEngine.zip NetworkEngine\*.*

pause