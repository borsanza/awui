Windows:
  winget install --id=TortoiseHg.TortoiseHg  -e
  winget install --id=Kitware.CMake  -e
  winget install --id=Ninja-build.Ninja  -e
  winget install --id=MSYS2.MSYS2  -e

  C:\msys64\mingw32.exe
    pacman -Syu
    pacman -S vim
    pacman -S mingw-w64-i686-toolchain
    pacman -S mingw-w64-i686-glew
    pacman -S mingw-w64-i686-SDL2
    pacman -S mingw-w64-i686-SDL2_image
    pacman -S mingw-w64-i686-cairo
    pacman -S mingw-w64-i686-nlohmann-json

  C:\msys64\mingw64.exe
    pacman -Syu
    pacman -S vim
    pacman -S mingw-w64-x86_64-toolchain
    pacman -S mingw-w64-x86_64-glew
    pacman -S mingw-w64-x86_64-SDL2
    pacman -S mingw-w64-x86_64-SDL2_image
    pacman -S mingw-w64-x86_64-cairo
    pacman -S mingw-w64-x86_64-nlohmann-json

    cd /c/awui/ext/
    ./generate-libs.sh

  Command:
    bbr.bat
