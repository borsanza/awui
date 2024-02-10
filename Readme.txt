Windows:
  winget install --id=TortoiseHg.TortoiseHg  -e
  winget install --id=Kitware.CMake  -e
  winget install --id=Ninja-build.Ninja  -e
  winget install --id=MSYS2.MSYS2  -e

  C:\msys64\mingw32.exe
    pacman -Syu
    pacman -S vim mingw-w64-i686-toolchain mingw-w64-i686-glew mingw-w64-i686-SDL2 mingw-w64-i686-SDL2_image mingw-w64-i686-cairo

  C:\msys64\mingw64.exe
    pacman -Syu
    pacman -S vim mingw-w64-x86_64-toolchain mingw-w64-x86_64-glew mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-cairo

    cd /c/awui/ext/
    ./generate-libs.sh

  Command:
    bbr.bat
