set  	CURRENT_PATH=make-commands
call 	clean-build.bat Y
mkdir 	..\build
cd		..\build
call 	"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
cmake 	-G"NMake Makefiles" ^
		-DCMAKE_BUILD_TYPE=Release ^
		-DCMAKE_VERBOSE_MAKEFILE=OFF ^
		-DPLATFORM=X64 ^
		-DCOMPILER=VS2015 ^
		-DBUILD_GPU=OFF ..
cd 		..\%CURRENT_PATH% 
pause

