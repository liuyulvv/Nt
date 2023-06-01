/**
 * @file NCanvasTest.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-06-01
 */

#include "NApplication.h"
#include "NCanvas.h"

int main() {
    NApplication app;
    NCanvas canvas;
    canvas.Show();
    app.Exec();
    return 0;
}