cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
project(AngleUWPExample)

add_executable(AngleUWPExample WIN32
    App.cpp
    main.cpp
    GLRendererImpl.cpp
    OpenGLES.cpp
    SimpleRenderer.cpp
    pch.cpp
)

target_compile_options(AngleUWPExample PRIVATE "-await")
target_compile_definitions(AngleUWPExample PRIVATE UNICODE)

if (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
    message(FATAL_ERROR [=[
        CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
        For building UWPs from CMake -DCMAKE_SYSTEM_NAME=WindowsStore is required. Please run the command line.
        <cmake> -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION="10.0" -DVCPKG_ROOT=<vcpkgpath>/vcpkg
    ]=])
endif()
set(CMAKE_SYSTEM_NAME "WindowsStore" CACHE INTERNAL "" FORCE )


if (NOT DEFINED VCPKG_ROOT)
    message(FATAL_ERROR [=[
    AngleUWPExample requires angle to be built for UWP. Please run the following commands to get angle built with vcpkg and then rerun with
    <cmake> -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION="10.0" -DVCPKG_ROOT=<vcpkgpath>/vcpkg
    
    To get vcpkg and build angle for UWP. Please run the following commands in <vcpkgpath>
    
    git clone https://github.com/Microsoft/vcpkg
    vcpkg/bootstrap-vcpkg.bat
    vcpkg/vcpkg install angle:x64-uwp
    ]=])
endif()


if (NOT EXISTS ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
    message(FATAL_ERROR "Cannot find vcpkg toolchain file at ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake. Please check the VCPKG_ROOT variable")
endif()

set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE PATH "")
include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

find_package(unofficial-angle REQUIRED)

target_link_libraries(AngleUWPExample PRIVATE unofficial::angle::libEGL unofficial::angle::libGLESv2)

target_link_libraries(AngleUWPExample PRIVATE WindowsApp.lib rpcrt4.lib onecoreuap.lib kernel32.lib)
set_target_properties(AngleUWPExample PROPERTIES VS_GLOBAL_MinimalCoreWin "true")

# Hack to make sure the DLLs are packaged along with the app
set(EGL_DLL ${VCPKG_ROOT}/installed/x64-uwp/bin/libEGL.dll)
set(GLES_DLL ${VCPKG_ROOT}/installed/x64-uwp/bin/libGLESv2.dll)
if (NOT EXISTS ${EGL_DLL})
    message(FATAL_ERROR "Cannot find ${EGL_DLL})
endif()
if (NOT EXISTS ${GLES_DLL})
    message(FATAL_ERROR "Cannot find ${GLES_DLL})
endif()

target_sources(AngleUWPExample PRIVATE ${EGL_DLL} ${GLES_DLL})
set_property(SOURCE ${EGL_DLL} ${GLES_DLL} PROPERTY VS_DEPLOYMENT_CONTENT 1)
set_property(SOURCE ${EGL_DLL} ${GLES_DLL} PROPERTY VS_DEPLOYMENT_LOCATION ".")

if (MSVC)
    target_compile_options(AngleUWPExample PRIVATE -permissive-)
    target_compile_options(AngleUWPExample PRIVATE -W4)
    target_compile_options(AngleUWPExample PRIVATE -WX)
    target_compile_options(AngleUWPExample   PRIVATE -EHsc)

    set_target_properties(AngleUWPExample PROPERTIES COMPILE_FLAGS "/Yupch.h")
    set_source_files_properties(pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
endif()