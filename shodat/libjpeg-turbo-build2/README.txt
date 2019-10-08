How to build libjpeg-turbo 

Requirements:
 -- CMake
 -- NASM
 -- mingw

1. make a directory 'libjpeg-turbo' in the same level as the other projects and get source:
	
	mkdir libjpeg-turbo
	cd libjpeg-turbo
	svn co svn://svn.code.sf.net/p/libjpeg-turbo/code/branches/1.2.x libjpeg-turbo-1.2.x
	
2. cd into libjpeg-turbo-1.2.x and run 'autoreconf -fiv'

3. cd back up (cd ..) and make a subdirectory for each target, e.g., mkdir build-win32

4. cd into the target dir (created above)

5. copy the script for the target into this directory.  For building to Win32 on linux 
it is make-libjpeg-turbo.sh.linux

6. make it executable (chmod a+x) and run it

7. run 'make'

++++++++++ On Linux for Linux: +++++++++++++

1. Do steps 1, 2 & 3 above if you have not already

2. cd into the dir you created for this linux target

3. ../libjpeg-turbo-1.2.x/configure --with-pic

4. make



NOTE: If you build activex-scraper or plugin-scraper you may get an error about 
'../libjpeg-turbo/jmorecfg.h:162: error: conflicting declaration "typedef long int INT32" To fix
this, comment out lines 161-163 in jmoreconfig and the native scrapers should now build no problem

NOTE: Mingw64 may need to be built from scratch. To do so, grab

http://mingw-w64.svn.sourceforge.net/viewvc/mingw-w64/experimental/buildsystem/makebuildroot.mk

and run 'make -f makebuildroot.mk' Then customize make-libjpeg-turbo.sh.linux and follow the 
instructions above