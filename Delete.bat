rmdir /s /q "ipch"
rmdir /s /q "Debug"
rmdir /s /q "Release"

rmdir /s /q .\Client\Include\Debug
rmdir /s /q .\Client\Include\ipch
rmdir /s /q .\Client\Include\Release
rmdir /s /q .\Client\Include\x64

rmdir /s /q .\Tool\Include\Debug
rmdir /s /q .\Tool\Include\ipch
rmdir /s /q .\Tool\Include\Release
rmdir /s /q .\Tool\Include\x64

rmdir /s /q .\Engine\System\Include\Debug
rmdir /s /q .\Engine\Utility\Include\Debug

del /f /q .\Client\Bin\Client.exe
del /f /q .\Client\Bin\Client.ilk
del /f /q .\Client\Bin\Client.pdb

del /f /q .\Tool\Bin\Client.exe
del /f /q .\Tool\Bin\Client.ilk
del /f /q .\Tool\Bin\Client.pdb

del /f /q FrameworkC.VC.db
