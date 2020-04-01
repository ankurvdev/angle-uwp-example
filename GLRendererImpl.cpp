#include "pch.h"

#include "GLRenderer.h"
#include "OpenGLES.h"
#include "SimpleRenderer.h"

#include <mutex>

struct GLRendererPrivate : GLRenderer
{
    public:
    GLRendererPrivate();
    ~GLRendererPrivate();

    void Start(const winrt::Windows::UI::Xaml::Controls::SwapChainPanel& withVisual);

    private:
    void RenderLoop();
    void CreateRenderSurface();
    void DestroyRenderSurface();
    void RecoverFromLostDevice();
    void StartRenderLoop();
    void StopRenderLoop();

    OpenGLES* mOpenGLES;

    EGLSurface                                         mRenderSurface{EGL_NO_SURFACE};
    bool                                               _stopRenderLoop{false};
    std::thread                                        _renderLoopThread;
    std::mutex                                         _mutex;
    winrt::Windows::UI::Xaml::Controls::SwapChainPanel mHostVisual{nullptr};
};

GLRendererPrivate::GLRendererPrivate() : mRenderSurface(EGL_NO_SURFACE)
{
    // TODO smartpointer
    mOpenGLES = new OpenGLES();
}

GLRendererPrivate::~GLRendererPrivate()
{
    StopRenderLoop();
    DestroyRenderSurface();
    delete mOpenGLES;
}

void GLRendererPrivate::Start(const winrt::Windows::UI::Xaml::Controls::SwapChainPanel& withVisual)
{
    mHostVisual = withVisual;
    CreateRenderSurface();
    StartRenderLoop();
}

void GLRendererPrivate::CreateRenderSurface()
{
    if (mOpenGLES && mRenderSurface == EGL_NO_SURFACE)
    {
        mRenderSurface = mOpenGLES->CreateSurface(mHostVisual);
    }
}

void GLRendererPrivate::DestroyRenderSurface()
{
    if (mOpenGLES)
    {
        mOpenGLES->DestroySurface(mRenderSurface);
    }
    mRenderSurface = EGL_NO_SURFACE;
}

void GLRendererPrivate::RecoverFromLostDevice()
{
    // Stop the render loop, reset OpenGLES, recreate the render surface
    // and start the render loop again to recover from a lost device.

    StopRenderLoop();

    {
        std::lock_guard guard(_mutex);

        DestroyRenderSurface();
        mOpenGLES->Reset();
        CreateRenderSurface();
    }

    StartRenderLoop();
}

void GLRendererPrivate::StartRenderLoop()
{
    _stopRenderLoop   = false;
    _renderLoopThread = std::thread([this] { this->RenderLoop(); });
}

void GLRendererPrivate::RenderLoop()
{
    std::lock_guard guard(_mutex);

    mOpenGLES->MakeCurrent(mRenderSurface);
    SimpleRenderer renderer;

    while (!_stopRenderLoop)
    {
        EGLint panelWidth  = 0;
        EGLint panelHeight = 0;
        mOpenGLES->GetSurfaceDimensions(mRenderSurface, &panelWidth, &panelHeight);

        // Logic to update the scene could go here
        renderer.UpdateWindowSize(panelWidth, panelHeight);
        renderer.Draw();

        // The call to eglSwapBuffers might not be successful (i.e. due to Device Lost)
        // If the call fails, then we must reinitialize EGL and the GL resources.
        if (mOpenGLES->SwapBuffers(mRenderSurface) != GL_TRUE)
        {
            // TODO: implement this
            throw;
            //// XAML objects like the SwapChainPanel must only be manipulated on the UI thread.
            // swapChainPanel->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new
            // Windows::UI::Core::DispatchedHandler([=]()
            //{
            //	RecoverFromLostDevice();
            //}, CallbackContext::Any));

            return;
        }
    }
}

void GLRendererPrivate::StopRenderLoop()
{
    _stopRenderLoop = true;
    _renderLoopThread.join();
}

std::unique_ptr<GLRenderer> GLRenderer::Create(const winrt::Windows::UI::Xaml::Controls::SwapChainPanel& panel)
{
    auto ptr = new GLRendererPrivate();
    ptr->Start(panel);
    return std::unique_ptr<GLRenderer>(ptr);
}