cd %~DP0
rmdir /q /s package
md package
cd package
xcopy ..\..\..\release\networkengine.exe
xcopy /s ..\data\*.* .\data\
xcopy ..\..\..\..\runtime\*.dll .
xcopy ..\*.dll .
xcopy /s ..\..\..\..\runtime\configs\*.* .\configs\
xcopy /s ..\..\..\..\runtime\data\vrlib\*.* .\data\vrlib\

echo NetworkEngine.exe --config configs/sim.json --config configs/vive-sim.json > sim.bat
echo NetworkEngine.exe --config configs/openvr.json > vive.bat


pause