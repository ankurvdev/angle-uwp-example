#pragma once

#include <winrt/Windows.UI.Xaml.Controls.h>

#include <memory>

struct GLRenderer
{
    virtual ~GLRenderer() = default;
    static std::unique_ptr<GLRenderer> Create(const winrt::Windows::UI::Xaml::Controls::SwapChainPanel& withVisual);
};
