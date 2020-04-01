#pragma once

#include <angle_gl.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

// EGL includes
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

class OpenGLES
{
    public:
    OpenGLES();
    ~OpenGLES();

    EGLSurface CreateSurface(winrt::Windows::UI::Xaml::Controls::SwapChainPanel& visual);
    void       GetSurfaceDimensions(const EGLSurface surface, EGLint* width, EGLint* height);
    void       DestroySurface(const EGLSurface surface);
    void       MakeCurrent(const EGLSurface surface);
    EGLBoolean SwapBuffers(const EGLSurface surface);
    void       Reset();

    private:
    void Initialize();
    void Cleanup();

    private:
    EGLDisplay mEglDisplay;
    EGLContext mEglContext;
    EGLConfig  mEglConfig;
};
