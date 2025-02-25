#define UNICODE
#include <windows.h>
#include <windowsx.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cstring>
#include <memory>


HWND hwnd;

const wchar_t* CLASSNAME = L"myapp_window";
uint32_t win_width = 640;
uint32_t win_height = 480;
HBITMAP hBitmap = nullptr;
void* pBits = nullptr;
bool is_drawing = false;
HDC hdcOffscreen = nullptr; // Device context for the off-screen bitmap

auto CreateBitmapFromRGB(char* pData, int width, int height)
    -> std::pair<HBITMAP, void*> {
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative -> top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24; //24-bit RGB
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC hdc = GetDC(nullptr);
    void* pBits;
    HBITMAP hbm = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    if(hbm != nullptr) {
        std::memcpy(pBits, pData, width * height * 3); //RGB
    }
    ReleaseDC(nullptr, hdc);
    return {hbm, pBits};
}

void InitializeOffScreenDC(HWND hwnd) {
    std::unique_ptr<char []> raw_data(new char[win_width * win_height * 3]);

    memset(raw_data.get(), 0x0, win_width * win_height * 3);
    std::ifstream ifs("../Rendered_Images/sample.pbm", std::ios::binary);
    std::string header;
    int width, height, bpp;
    ifs >> header;
    ifs >> width >> height >> bpp;
    ifs.ignore();
    ifs.read(raw_data.get(), win_width * win_height * 3);
    for (uint32_t i = 0; i < win_width * win_height * 3; i+=3) {
        std::swap(raw_data[i], raw_data[i+2]);
    }
    ifs.close();

    auto bitmap_data = CreateBitmapFromRGB(raw_data.get(), win_width, win_height);
    hBitmap = bitmap_data.first;
    pBits = bitmap_data.second;

    HDC hdc = GetDC(hwnd);
    hdcOffscreen = CreateCompatibleDC(hdc);
    SelectObject(hdcOffscreen, hBitmap);
    ReleaseDC(hwnd, hdc);
}

void CleanupOffScreenDC() {
    if (hdcOffscreen) DeleteDC(hdcOffscreen); // Delete off-screen DC
}

void SetPixelColor(void* pBits, int width, int x, int y, uint8_t red, uint8_t green, uint8_t blue) {
    if(!pBits) return; // Ensure valid pointer

    int pixel_index = (y * width + x) * 3; //RGB bits

    uint8_t* pPixel = static_cast<uint8_t*>(pBits) + pixel_index;

    pPixel[0] = blue;
    pPixel[1] = green;
    pPixel[2] = red;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CLOSE: // Closing Window
            if(hBitmap != nullptr) {
                DeleteObject(hBitmap);
                hBitmap = nullptr;
            }
            CleanupOffScreenDC();
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY: // Program gets Terminated
            PostQuitMessage(0);
            break;
        case WM_LBUTTONDOWN:
            is_drawing = true;
            break;
        case WM_LBUTTONUP:
            is_drawing = false;
            break;
        case WM_MOUSEMOVE: {
            int xpos = GET_X_LPARAM(lParam);
            int ypos = GET_Y_LPARAM(lParam);
            if(is_drawing) {
                SetPixelColor(pBits, win_width, xpos, ypos, 255, 0, 0);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        case WM_ERASEBKGND:
            return 1; // Indicate backround erase is handled
        case WM_PAINT: // Repaint window if drawn or obscured by another one
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                BitBlt(hdc, 0, 0, win_width, win_height, hdcOffscreen, 0, 0, SRCCOPY);
                EndPaint(hWnd, &ps);
            }
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

void CreateAndRegisterWindow(HINSTANCE hInstance) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc; // pointer to function which processes messages sent to the window
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASSNAME; // specifies window class name
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // Set default arrow cursor
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION); // Load default app icon
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.hIconSm = LoadIcon(hInstance, IDI_APPLICATION); // Load small icon for app

    if(!RegisterClassEx(&wc)) {
        MessageBox(nullptr, L"Window Registration Failed", L"Error",
            MB_ICONEXCLAMATION | MB_OK);
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        CLASSNAME, 
        L"Foo", 
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & WS_MAXIMIZEBOX, //non-resizable
        CW_USEDEFAULT, CW_USEDEFAULT, win_width, win_height,
        nullptr, nullptr, hInstance, nullptr);
    
    if(hwnd == nullptr) {
        MessageBox(nullptr, L"Window Creation Failed", L"Error", 
            MB_ICONEXCLAMATION | MB_OK);
    }

    InitializeOffScreenDC(hwnd);

    ShowWindow(hwnd, SW_SHOWDEFAULT); // or WS_VISIBLE, this has more options
    UpdateWindow(hwnd);
}

void DoSomeWork() {

}

int main(int argc, char** argv) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    CreateAndRegisterWindow(hInstance);
    MSG msg;
    while(1) {
        while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if(msg.message == WM_QUIT) {
                break;
            }
        }
        if(msg.message == WM_QUIT)
            break;
        DoSomeWork();
    }
    return 0;
}