How to compile in Windows:

Install CMake from http://www.cmake.org/

Install Tortoise hg from http://tortoisehg.bitbucket.org/


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

3.- Set the default project to awuiDemo