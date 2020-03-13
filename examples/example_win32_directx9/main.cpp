// dear imgui: standalone example application for DirectX 9
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

#include <stdio.h>          // vsnprintf, sscanf, printf
#ifdef _MSC_VER
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif
// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    bool popupclose_flag = false;
    static bool iSwitchWinMaximizeRestore = false;
    if (0)
    {
        #pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")
    }
    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    //HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX9 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX9 Example"), WS_POPUP, 100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                //ShowExampleMenuFile();
                ImGui::MenuItem("(dummy menu)", NULL, false, false);
                if (ImGui::MenuItem("Quit", "Alt+F4"))
                {
                    ::SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
                    //ImGui::OpenPopup("Close?");
                }                
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Max"))
                {                    
                    ::PostMessage(hwnd, WM_SYSCOMMAND, !iSwitchWinMaximizeRestore ? SC_MAXIMIZE : SC_RESTORE, 0);
                    iSwitchWinMaximizeRestore = !iSwitchWinMaximizeRestore;
                }
                if (ImGui::MenuItem("Min"))
                {
                    ::SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
                }
                if (ImGui::MenuItem("Close"))
                {
                    ::SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
                }
                ImGui::EndMenu();
            }            
            /*ImGui::Begin("Hello, world!中文1");
            ImGui::End();*/
            char buf[128];
            //sprintf(buf, "Animated title %c %d###AnimatedTitle", "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], ImGui::GetFrameCount());
            //sprintf(buf, "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            sprintf(buf, "(%.1f FPS)", ImGui::GetIO().Framerate);
            ImGui::MenuItem(buf, NULL, false, false);
            if (ImGui::MenuItem("x"))
            {
                ::SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
            }
            if (ImGui::MenuItem("+"))
            {
                ::PostMessage(hwnd, WM_SYSCOMMAND, !iSwitchWinMaximizeRestore ? SC_MAXIMIZE : SC_RESTORE, 0);
                iSwitchWinMaximizeRestore = !iSwitchWinMaximizeRestore;
            }
            if (ImGui::MenuItem("-"))
            {
                ::SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
            }            
            ImGui::EndMainMenuBar();
        }

        for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) //if (io.KeysDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]); }
        {
            if (io.KeysDownDuration[i] >= 0.0f && i == 27)
            {
                if (!popupclose_flag)
                {
                    popupclose_flag = true;
                    ImGui::OpenPopup("Close?");
                }
                
            }
        }

        if (ImGui::BeginPopupModal("Close?", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::Text("Application will be closed.\nThis operation cannot be undone!\n\n");
            ImGui::Separator();

            //static int dummy_i = 0;
            //ImGui::Combo("Combo", &dummy_i, "Delete\0Delete harder\0");

            /*static bool dont_ask_me_next_time = false;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            ImGui::PopStyleVar();*/

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                ::SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);               
            }
            ImGui::SetItemDefaultFocus();                
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                popupclose_flag = false;
            }
            ImGui::EndPopup();
            
        }

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGuiWindowFlags window_flags_ = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

            ImGui::Begin("Hello, world!中文", NULL, window_flags_);                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 0.0f;
            //ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 0.0f, "%.0f");

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            {
                counter++;
                //::PostQuitMessage(0);
                //::SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
            }
            ImGui::SameLine();
            if (ImGui::Button("Min"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            {
                //::PostQuitMessage(0);
                ::SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
            }
            ImGui::SameLine();
            if (ImGui::Button("Max"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            {
                //::PostQuitMessage(0);
                //::SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*255.0f), (int)(clear_color.y*255.0f), (int)(clear_color.z*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static POINT pt, pe;
    static RECT rt, re;
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_CREATE:
        {
            //int scrWidth, scrHeight;
            //RECT rect;
            ////获得屏幕尺寸
            //scrWidth = GetSystemMetrics(SM_CXSCREEN);
            //scrHeight = GetSystemMetrics(SM_CYSCREEN);
            ////取得窗口尺寸
            //GetWindowRect(hWnd, &rect);
            ////重新设置rect里的值
            //rect.left = (scrWidth - rect.right + rect.left) / 2;
            //rect.top = (scrHeight - rect.bottom + rect.top) / 2;
            //rect.right = rect.right - rect.left;    // 保存窗口宽度
            //rect.bottom = rect.bottom - rect.top; // 保存窗口高度
            ////移动窗口到指定的位置
            //SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);

            RECT rect;
            int	xLeft, yTop;
            GetWindowRect(hWnd, &rect);
            /*xLeft = (GetSystemMetrics(SM_CXFULLSCREEN) - (rect.right - rect.left)) / 2;
            yTop = (GetSystemMetrics(SM_CYFULLSCREEN) - (rect.bottom - rect.top)) / 2;*/
            xLeft = (GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
            yTop = (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;
            // Move the window to the correct coordinates with SetWindowPos()
            //bResult = SetWindowPos(hWnd,HWND_TOP,xLeft,yTop,-1,-1,SWP_NOSIZE|SWP_NOZORDER/*|SWP_NOACTIVATE*/);
            SetWindowPos(hWnd, HWND_TOPMOST, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER);
            SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        }
        break;
    case WM_RBUTTONDOWN:
        SetCapture(hWnd);      // 设置鼠标捕获(防止光标跑出窗口失去鼠标热点)
        GetCursorPos(&pt);      // 获取鼠标光标指针当前位置
        GetWindowRect(hWnd, &rt);  // 获取窗口位置与大小
        re.right = rt.right - rt.left;    // 保存窗口宽度
        re.bottom = rt.bottom - rt.top; // 保存窗口高度
        break;
    case WM_RBUTTONUP:
        ReleaseCapture();      // 释放鼠标捕获，恢复正常状态
        break;
    case WM_MOUSEMOVE:
        GetCursorPos(&pe);     // 获取光标指针的新位置
        if (wParam == MK_RBUTTON)    // 当鼠标右键按下
        {
            re.left = rt.left + (pe.x - pt.x);  // 窗口新的水平位置
            re.top = rt.top + (pe.y - pt.y); // 窗口新的垂直位置
            MoveWindow(hWnd, re.left, re.top, re.right, re.bottom, true); // 移动窗口
        }
        break;
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
