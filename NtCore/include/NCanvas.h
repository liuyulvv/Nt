#pragma once

/**
 * @file NCanvas.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#include <string>

#include "NPlatform.h"
#include "NPosition.h"
#include "NSize.h"

class BDllExport NCanvas {
public:
    NCanvas();
    ~NCanvas() = default;
    NCanvas(const NCanvas& canvas) = delete;
    NCanvas(NCanvas&& canvas) = delete;
    NCanvas& operator=(const NCanvas& canvas) = delete;
    NCanvas& operator=(NCanvas&& canvas) = delete;

public:
    void Show() const;
    std::wstring Title() const;
    void SetTitle(const std::wstring& title);
    void Move(const NPosition& pos, const NSize& size, bool repaint = false);
    void Move(const NPosition& pos, bool repaint = false);
    void Move(int32_t x, int32_t y, uint32_t width, uint32_t height, bool repaint = false);
    void Move(int32_t x, int32_t y, bool repaint = false);
    void Resize(const NSize& size, bool repaint = false);
    void Resize(uint32_t width, uint32_t height, bool repaint = false);

public:
    void MoveEvent(const NPosition& pos);
    void ResizeEvent(const NSize& size);

private:
    NSize GetMonitorSize() const;

private:
    NCanvasID id_{};
    std::wstring title_{};
    NPosition position_{};
    NSize size_{};
};