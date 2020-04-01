if not defined DevEnvDir (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
)
if NOT EXIST vcpkg/vcpkg.exe (
	git clone https://github.com/Microsoft/vcpkg
	vcpkg/bootstrap-vcpkg.bat
)

.\vcpkg\vcpkg.exe install angle:x64-uwp

cmake -DVCPKG_ROOT=%CD%/vcpkg -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION="10.0" %~dp0
cmake --open .