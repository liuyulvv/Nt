/**
 * @file NCanvas.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#include "NCanvas.h"

#include <stdexcept>

NCanvas::NCanvas() {
#if defined(_WIN32)
    id_ = CreateWindowEx(
        0,
        N_CLASS_NAME,
        title_.c_str(),
        WS_OVERLAPPEDWINDOW,
        position_.x_,
        position_.y_,
        size_.width_,
        size_.height_,
        nullptr,
        nullptr,
        nullptr,
        nullptr);
    if (!id_) {
        throw std::runtime_error("Failed to create canvas");
    }
    SetWindowLongPtr(id_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
#endif
    auto size = GetMonitorSize();
    Move({static_cast<int32_t>(size.width_ / 4), static_cast<int32_t>(size.height_ / 4)}, {size.width_ / 2, size.height_ / 2});
}

NSize NCanvas::GetMonitorSize() const {
    NSize size{};
#if defined(_WIN32)
    auto monitor = MonitorFromWindow(id_, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEX info;
    info.cbSize = sizeof(info);
    if (!GetMonitorInfo(monitor, &info))
        return size;
    DEVMODE dm{};
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    if (!EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &dm))
        return size;
    size.width_ = dm.dmPelsWidth;
    size.height_ = dm.dmPelsHeight;
#endif
    return size;
}

void NCanvas::Show() const {
#if defined(_WIN32)
    ShowWindow(id_, 5);
#endif
}

std::wstring NCanvas::Title() const {
    return title_;
}

void NCanvas::SetTitle(const std::wstring& title) {
    title_ = title;
#if defined(_WIN32)
    SetWindowText(id_, title_.c_str());
#endif
}

void NCanvas::Move(const NPosition& pos, const NSize& size, bool repaint) {
#if defined(_WIN32)
    MoveWindow(id_, pos.x_, pos.y_, size.width_, size.height_, repaint);
#endif
}

void NCanvas::Move(const NPosition& pos, bool repaint) {
    Move(pos, size_, repaint);
}

void NCanvas::Move(int32_t x, int32_t y, uint32_t width, uint32_t height, bool repaint) {
    Move({x, y}, {width, height}, repaint);
}

void NCanvas::Move(int32_t x, int32_t y, bool repaint) {
    Move({x, y}, repaint);
}

void NCanvas::Resize(const NSize& size, bool repaint) {
    Move(position_, size, repaint);
}

void NCanvas::Resize(uint32_t width, uint32_t height, bool repaint) {
    Resize({width, height}, repaint);
}

void NCanvas::MoveEvent(const NPosition& pos) {
    position_ = pos;
}

void NCanvas::ResizeEvent(const NSize& size) {
    size_ = size;
}
