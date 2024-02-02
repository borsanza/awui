Windows:
  winget install --id=Kitware.CMake  -e
  winget install --id=Ninja-build.Ninja  -e
  Install MinGW-w64 using MSYS2: https://www.msys2.org/
    pacman -Syu
    pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-glew mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-cairo

  buildvars.bat
  cd libawui
  cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release .
  cd ..
  cd samples
  cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release .
  ninja

Ubuntu:
  apt-get install cmake libsdl1.2-dev libsdl-image1.2-dev libcairo2-dev libglew-dev

openssl s_client -connect code.google.com:443 < /dev/null 2> /dev/null | openssl x509 -fingerprint -noout -in /dev/stdin

How to compile in Windows:

Install CMake from http://www.cmake.org/

Install Tortoise hg from http://tortoisehg.bitbucket.org/


Install Oracle Java 8:

https://tecadmin.net/install-oracle-java-8-ubuntu-via-ppa/#

add-apt-repository ppa:webupd8team/java
apt-get update
apt-get install oracle-java8-installer
java -version

apt-get install oracle-java8-set-default

cat >> /etc/environment <<EOL
JAVA_HOME=/usr/lib/jvm/java-8-oracle
JRE_HOME=/usr/lib/jvm/java-8-oracle/jre
EOL

awui:

1.- Clone the repository from google code:
	hg clone https://borsanza@awui.googlecode.com/hg/ awui


Cairo:

1.- Go to: http://www.gtk.org/download-windows.html

2.- Download and decompress in c:\awui-project\cairo the next packages:

	cairo_xx_win32.zip (Run-time)
	cairo-dev_xx_win32.zip (Dev)
	expat_xx_win32.zip (Run-time)
	fontconfig_xx_win32.zip (Run-time)
	freetype_xx_win32.zip (Run-time)
	libpng_xx_win32.zip (Run-time)
	zlib_xx_win32.zip (Run-time)

3.- Copy the dll files to the executable path.


Libaw:

1.- Clone the repository from google code.
	hg clone https://libaw.googlecode.com/hg/ c:\awui-project\libaw

2.- Run the script C:\awui-project\awui\libaw\generate.bat


Finish:

1.- Run the script C:\awui-project\awui\clean.bat

2.- Open the Project.sln with Visual Studio

3.- Set the at startup project to awuiDemo

