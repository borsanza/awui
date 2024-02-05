Windows:
  winget install --id=Kitware.CMake  -e
  winget install --id=Ninja-build.Ninja  -e
  Install MinGW-w64 using MSYS2: https://www.msys2.org/
    pacman -Syu
    pacman -S mingw-w64-x86_64-toolchain
    pacman -S mingw-w64-x86_64-glew
    pacman -S mingw-w64-x86_64-SDL2
    pacman -S mingw-w64-x86_64-SDL2_image
    pacman -S mingw-w64-x86_64-cairo

  bbr.bat
