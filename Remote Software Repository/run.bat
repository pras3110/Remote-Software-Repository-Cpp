cd Debug
start ServerPrototype.exe
timeout /t 1
cd "../Client/bin/x86/Debug"
start Client.exe
timeout /t 58
start Client.exe -port 8083