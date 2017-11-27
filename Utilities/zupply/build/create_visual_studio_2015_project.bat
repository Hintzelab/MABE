mkdir vc14
cd vc14
cmake -G "Visual Studio 14 2015 Win64" ..
cmake --build . --config Release --target unittest
cd ..
copy /Y .\vc14\Release\unittest.exe .\unittest.exe 