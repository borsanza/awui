@ECHO OFF

mkdir installer
mkdir installer\images

copy images\button.png installer\images
copy libawui.dll installer\
copy stationTV.exe installer\
copy C:\msys64\mingw64\bin\SDL.dll installer\
copy C:\msys64\mingw64\bin\SDL_image.dll installer\
copy C:\msys64\mingw64\bin\glew32.dll installer\
copy C:\msys64\mingw64\bin\libbz2-1.dll installer\
copy C:\msys64\mingw64\bin\libcairo-2.dll installer\
copy C:\msys64\mingw64\bin\libexpat-1.dll installer\
copy C:\msys64\mingw64\bin\libfontconfig-1.dll installer\
copy C:\msys64\mingw64\bin\libfreetype-6.dll installer\
copy C:\msys64\mingw64\bin\libgcc_s_seh-1.dll installer\
copy C:\msys64\mingw64\bin\libglib-2.0-0.dll installer\
copy C:\msys64\mingw64\bin\libgraphite2.dll installer\
copy C:\msys64\mingw64\bin\libharfbuzz-0.dll installer\
copy C:\msys64\mingw64\bin\libiconv-2.dll installer\
copy C:\msys64\mingw64\bin\libintl-8.dll installer\
copy C:\msys64\mingw64\bin\libjpeg-8.dll installer\
copy C:\msys64\mingw64\bin\liblzma-5.dll installer\
copy C:\msys64\mingw64\bin\libpcre-1.dll installer\
copy C:\msys64\mingw64\bin\libpixman-1-0.dll installer\
copy C:\msys64\mingw64\bin\libpng16-16.dll installer\
copy C:\msys64\mingw64\bin\libstdc*-6.dll installer\
copy C:\msys64\mingw64\bin\libtiff-5.dll installer\
copy C:\msys64\mingw64\bin\libwebp-7.dll installer\
copy C:\msys64\mingw64\bin\libwinpthread-1.dll installer\
copy C:\msys64\mingw64\bin\libzstd.dll installer\
copy C:\msys64\mingw64\bin\zlib1.dll installer\

pause