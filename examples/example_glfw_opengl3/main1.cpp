// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
//#include "SequenceFrame.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int loadtexture(char *path , bool flip)
{
    // load and create a texture 
    // -------------------------
    unsigned int texture;
    //GLuint texture;
    //texture = matToTexture(frame, GL_LINEAR, GL_LINEAR, GL_CLAMP);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, frame.ptr()); // GL_BGR_EXT
    ////glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.data);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.data);
    //glGenerateMipmap(GL_TEXTURE_2D);

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(flip); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        //std::cout << "Failed to load texture" << std::endl;
        printf("Failed to load texture\r\n");
    }
    stbi_image_free(data);
    return texture;
}

#include <shellapi.h>
#include <WinUser.h>

//void ToTray(HWND hWnd)
//{
//    NOTIFYICONDATA nid;
//    nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
//    nid.hWnd = hWnd;
//    nid.uID = IDR_MAINFRAME;
//    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
//    nid.uCallbackMessage = WM_TO_TRAY;//自定义的消息 处理托盘图标事件
//    nid.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_SMALL));
//    wcscpy(nid.szTip, _T("自定义程序名"));//鼠标放在托盘图标上时显示的文字
//    Shell_NotifyIcon(NIM_ADD, &nid);//在托盘区添加图标
//}
//
//void DeleteTray(HWND hWnd)
//{
//    NOTIFYICONDATA nid;
//    nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
//    nid.hWnd = hWnd;
//    nid.uID = IDR_MAINFRAME;
//    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
//    nid.uCallbackMessage = WM_TO_TRAY;//自定义的消息名称 处理托盘图标事件
//    nid.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_SMALL));
//    wcscpy_s(nid.szTip, _T("自定义程序名"));//鼠标放在托盘图标上时显示的文字
//    Shell_NotifyIcon(NIM_DELETE, &nid);//在托盘中删除图标
//}

#define ImgTotalnum 100
//#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup") //设置入口地址  

#include <dwmapi.h>
#pragma comment (lib, "dwmapi.lib")
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
HHOOK   _glh_hook_ = NULL;  //定义为全局变量

double currentTime, lastTime;

#define BACKTIME 300

LRESULT CALLBACK MouseProc(int nCode, WPARAM wparam, LPARAM lparam)
{
    if (nCode >= 0)
    {
        if (wparam == WM_RBUTTONDOWN)
        {
            lastTime = GetTickCount() * 0.001f;
        }
        else if (wparam == WM_LBUTTONDOWN)
        {
            lastTime = GetTickCount() * 0.001f;
        }
        else if (wparam == WM_MBUTTONDOWN)
        {
            lastTime = GetTickCount() * 0.001f;
        }
    }

    return CallNextHookEx(_glh_hook_, nCode, wparam, lparam);
}

BOOL FullScreenViewCtrl(HWND m_hWnd, //想要全屏的窗口句柄
    BOOL bFullScreen) //TURE进入全屏 FALSE退出全屏
{
    static HWND pParentWndSave = NULL; //父窗口句柄
    static DWORD dwWindowStyleSave = 0; //窗口风格
    static RECT rcWndRectSave = { 0, 0, 0, 0 }; //窗口位置
    static BOOL bEnterFullView = FALSE;  //进入全屏？

    _ASSERT(IsWindow(m_hWnd));

    if (bFullScreen)
    {
        if (!bEnterFullView)//非全屏模式
        {
            dwWindowStyleSave = GetWindowLong(m_hWnd, GWL_STYLE); //保存窗口风格
            GetWindowRect(m_hWnd, &rcWndRectSave); //保存窗口位置
            pParentWndSave = SetParent(m_hWnd, NULL); //保存父窗口句柄/设置父窗口
            SetWindowLong(m_hWnd, GWL_STYLE, dwWindowStyleSave & (~WS_CHILD) | WS_POPUP);//使窗口具有Popup风格
            SetWindowPos(m_hWnd, HWND_TOP,
                0, -20, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)+20,
                SWP_DRAWFRAME | SWP_DEFERERASE | SWP_FRAMECHANGED); //修改窗口置全屏
            bEnterFullView = TRUE; //进入全屏模式
        }
    }
    else
    {
        if (bEnterFullView) //是全屏模式
        {
            SetWindowLong(m_hWnd, GWL_STYLE, dwWindowStyleSave);//恢复窗口风格
            SetParent(m_hWnd, pParentWndSave); //恢复父窗口句柄
            POINT ptLT = { rcWndRectSave.left, rcWndRectSave.top };
            ScreenToClient(m_hWnd, &ptLT);
            MoveWindow(m_hWnd, ptLT.x, ptLT.y,
                rcWndRectSave.right - rcWndRectSave.left, rcWndRectSave.bottom - rcWndRectSave.top,
                TRUE); //恢复原始位置
            bEnterFullView = FALSE; //退出全屏模式
        }
    }

    return bEnterFullView;//返回是否是全屏？
}

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <io.h>
#include <iostream>
#include <string>
#include <Tlhelp32.h>
#include <algorithm>
#include <vector>

//LPCTSTR szAppClassName = TEXT("NIOSH Hearing Loss Simulator");
//LPCTSTR szAppWindowName = TEXT("NIOSH Hearing Loss Simulator");
LPCTSTR szAppClassName = TEXT("NIOSH听力损失模拟器");
LPCTSTR szAppWindowName = TEXT("NIOSH听力损失模拟器");
//NIOSH听力损失模拟器
using namespace std;

//隐藏DOS黑窗口
//#pragma comment(linker,"/subsystem:\"windows\"  /entry:\"mainCRTStartup\"" )

#pragma warning(disable : 4996)

//定义路径最大程度
#define MAX_PATH_NUM 4096
//定义守护进程名称
#define PROCCESS_NAME "HLSim.exe"
//定义写入的注册表路径
#define SELFSTART_REGEDIT_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\"

void kill(int pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    BOOL bOkey = TerminateProcess(hProcess, 0);
    if (bOkey)
    {
        WaitForSingleObject(hProcess, 1000);
        CloseHandle(hProcess);
        hProcess = NULL;
    }
}
//
void TerminateProcessByName(LPCWSTR name)
{
    DWORD pid = 0;

    // Create toolhelp snapshot.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);

    std::vector<DWORD> pids;
    // Walkthrough all processes.
    if (Process32FirstW(snapshot, &process))
    {
        do
        {
            // Compare process.szExeFile based on format of name, i.e., trim file path
            // trim .exe if necessary, etc.
            std::wstring szExeFile = process.szExeFile;
            std::wstring szName = name;
            /*std::transform(szExeFile.begin(), szExeFile.end(), szExeFile.begin(), towlower);
            std::transform(szName.begin(), szName.end(), szName.begin(), towlower);*/

            if (!wcscmp(szExeFile.c_str(), szName.c_str()))
            {
                pid = process.th32ProcessID;
                pids.push_back(pid);
            }
        } while (Process32NextW(snapshot, &process));
    }

    CloseHandle(snapshot);

    size_t size = pids.size();
    for (size_t i = 0; i < size; ++i)
    {
        kill(pids[i]);
    }
}

void changescsize(int cw, int ch)
{
    DEVMODE dmScreenSettings;
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize = sizeof(dmScreenSettings);
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
    dmScreenSettings.dmPelsWidth = cw;
    dmScreenSettings.dmPelsHeight = ch;
    long result = ChangeDisplaySettings(&dmScreenSettings, 0);
    if (result == DISP_CHANGE_SUCCESSFUL)
    {
        //MessageBox(NULL, TEXT("分辨率修改成功!"), TEXT("提示"), MB_ICONEXCLAMATION | MB_OK);
        cout << "分辨率修改成功！" << endl;
        ChangeDisplaySettings(&dmScreenSettings, CDS_UPDATEREGISTRY);
    }
    else
    {
        //MessageBox(NULL, TEXT("分辨率修改失败!"), TEXT("提示"), MB_ICONEXCLAMATION | MB_OK);
        cout << "分辨率修改失败！" << endl;
        ChangeDisplaySettings(NULL, 0);
    }

}

HWND hWndApp, hWndPopup, hWndPopup1;


HWND handle1;// = FindWindow(NULL, szAppWindowName);

int cxScreen, cyScreen;
char pPath[MAX_PATH_NUM] = { 0 };
STARTUPINFOA si;
//进程对象
PROCESS_INFORMATION pi;
char pCmd[MAX_PATH_NUM] = { 0 };

int exechandle1()
{
    if (_access(pPath, 0) != -1)
    {
        //创建子进程，判断是否执行成功
        //changescsize(800, 600);
        cxScreen = GetSystemMetrics(SM_CXSCREEN);//获得屏幕的宽度
        cyScreen = GetSystemMetrics(SM_CYSCREEN);//获得屏幕的高度
        //Sleep(1000);
        if (!CreateProcessA(NULL, pCmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        {
            cout << "守护进程启动失败，程序即将退出" << endl;
            return -1;
        }
        //else
        //{

        //Sleep(5000);

        //HWND hParentWnd = getProcessMainWndByProcessName("HLSim.exe");

        //HWND
        handle1 = FindWindow(NULL, szAppWindowName);

        for (int i = 0; i < 10; i++)
        {
            if (handle1 == NULL)
            {
                Sleep(1000);
                handle1 = FindWindow(NULL, szAppWindowName);
                cout << "cannotfindwindow, retry " << i << endl;
            }
            else
            {
                break;
            }
        }

        //SetWindowLong(handle1, GWL_STYLE, 0 & ~WS_CAPTION);
        //SetWindowLong(handle1, GWL_STYLE, 0);
        //ShowWindow(handle1, SW_HIDE);

        //cout << cxScreen << cyScreen << endl;

        //MoveWindow(handle1, 0, 0, cxScreen, cyScreen, true);
        //SetWindowPos(handle1,HWND_TOPMOST, 0, 0, cxScreen, cyScreen, SWP_SHOWWINDOW);//设置窗口为最顶层


        //DWORD dwStyle = GetStyle();//获取旧样式
        DWORD dwNewStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        DWORD basic_borderless = WS_POPUP | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
        //dwNewStyle &= dwStyle;//按位与将旧样式去掉
        SetWindowLong(handle1, GWL_STYLE, dwNewStyle);//设置成新的样式
        //SetWindowLong(handle1, GWL_STYLE, basic_borderless);
        //DWORD dwExStyle = GetExStyle();//获取旧扩展样式
        DWORD dwNewExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
        //dwNewExStyle &= dwExStyle;//按位与将旧扩展样式去掉  
        //Sleep(1000);
        SetWindowLong(handle1, GWL_EXSTYLE, dwNewExStyle);//设置新的扩展样式  
        //if (SetMenu(handle1, NULL) == 0)
        //{
            cout << "set menu " << SetMenu(handle1, NULL) << endl;
        //}
            cout << "draw menu " << DrawMenuBar(handle1) << endl;
        //SetWindowPos(handle1, NULL, 0, 0, 800, 600, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);//告诉windows：我的样式改变了，窗口位置和大小保持原来不变！  
        //SetWindowPos(handle1, NULL, 0, -20, cxScreen, cyScreen+20, SWP_SHOWWINDOW);
        SetWindowPos(handle1, HWND_TOPMOST, 0, -20, cxScreen, cyScreen + 20, SWP_SHOWWINDOW);
        //SetWindowPos(handle1, HWND_TOPMOST, 0, -20, cxScreen, cyScreen, SWP_SHOWWINDOW);
        //DWORD dwStyle = GetStyle();//获取旧样式
        //WORD dwNewStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        //dwNewStyle &= dwStyle;//按位与将旧样式去掉
        //SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);//设置成新的样式
        //DWORD dwExStyle = GetExStyle();//获取旧扩展样式
        //DWORD dwNewExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
        //dwNewExStyle &= dwExStyle;//按位与将旧扩展样式去掉
        //SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewExStyle);//设置新的扩展样式
        //SetWindowPos(handle1, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);//告诉windows：我的样式改变了，窗口位置和大小保持原来不变！
        //SetWindowLong(handle1, GWL_STYLE, GetWindowLong(handle1, GWL_STYLE) & ~WS_CAPTION);
        //SetWindowPos(handle1, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);

        //HANDLE  * wnd = FindWindow(NULL, "XX - 记事本");//窗口名完整
        //RECT r(0,0,800,600);
        //wnd->GetClientRect(r);
        //Sleep(1000);
        InvalidateRect(handle1, NULL, TRUE);
        //Sleep(1000);

        FullScreenViewCtrl(handle1, true);

        SetWindowPos(handle1, HWND_TOPMOST, 0, -20, cxScreen, cyScreen + 20, SWP_HIDEWINDOW);

        //}
        //启动成功，获取进程的ID
        cout << "守护进程成功，ID:" << pi.dwProcessId << endl;
        //无限等待子进程退出
        //WaitForSingleObject(pi.hProcess, INFINITE);
        //changescsize(cxScreen, cyScreen);
        //Sleep(5000);
        //如果退出了
        //cout << "守护进程退出了。。。" << endl;
        //关闭进程和句柄
        //CloseHandle(pi.hProcess);
        //CloseHandle(pi.hThread);
               
    }
    else
    {        
        cout << "守护程序不存在" << endl;
        return -1;
    }
    return 0;
}

int main(int, char**)
{
    //int cxScreen, cyScreen;
    cxScreen = GetSystemMetrics(SM_CXSCREEN);//获得屏幕的宽度
    cyScreen = GetSystemMetrics(SM_CYSCREEN);//获得屏幕的高度

    HANDLE hObject = CreateMutex(NULL, FALSE, "DeamonProcess");

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(hObject);
        //MessageBox(NULL,"程序已经运行!",NULL,NULL);  
        return FALSE;
    }

    SetConsoleTitle("DeamonProcess");

    //获得守护的进程的句柄
    HWND handle = FindWindow(NULL, szAppWindowName);
    if (handle != NULL)
    {
        //MessageBox(NULL, TEXT("Application is already running"), szAppClassName, MB_ICONERROR);
        //string strName(szAppClassName);
        string strName = (LPSTR)szAppClassName;
        //cout<< strName.c_str() <<" is already running." << endl;
        cout << strName << " is already running." << endl;

        string process_name = PROCCESS_NAME;
        size_t size = process_name.length();
        wchar_t* buffer = new wchar_t[size + 1];
        MultiByteToWideChar(CP_ACP, 0, process_name.c_str(), size, buffer, size * sizeof(wchar_t));
        buffer[size] = 0;  //确保以 '\0' 结尾 
                           //      ::MessageBox(NULL, buffer, NULL, NULL);


        TerminateProcessByName(buffer);
        delete buffer;
        buffer = NULL;
        //Sleep(3000);
        //ExitProcess(1);
    }

    //设置程序开机自启动
    /*if (!SetSelfStart())
    {
        cout << "守护进程开机自启动失败" << endl;
        return -1;
    }*/

    //STARTUPINFOA si;
    ////进程对象
    //PROCESS_INFORMATION pi;
    //初始化
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    //获取当前程序的路径
    //char pPath[MAX_PATH_NUM] = { 0 };
    GetCurrentDirectoryA(MAX_PATH_NUM, pPath);

    //拼接需要守护的程序
    strcat(pPath, "\\");
    strcat(pPath, PROCCESS_NAME);

    //构造cmd执行守护进程的字符串
    //char pCmd[MAX_PATH_NUM] = { 0 };
    //strcat(pCmd,"cmd /c ");
    strcat(pCmd, pPath);



    lastTime = 0.000f;

    HINSTANCE   glhInstance = NULL;
    glhInstance = GetModuleHandle(NULL);
    _glh_hook_ = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, glhInstance, 0);
    //WH_MOUSE_LL指监控鼠标行为,MouseProc是回调函数

    //glEnable(GL_BLEND);

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    //GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    bool isFullScreen = true;
    int monitorCount;
    //GLFWmonitor* pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;
    GLFWmonitor** pMonitor = isFullScreen ? glfwGetMonitors(&monitorCount) : NULL;

    //std::cout << "Screen number is " << monitorCount << std::endl;
    printf("Screen number is %d\r\n", monitorCount);
    //GLFWmonitor** pM = pMonitor;
    int holographic_screen = -1;
    int screen_x, screen_y;
    for (int i = 0; i < monitorCount; i++) {        
        const GLFWvidmode* mode = glfwGetVideoMode(pMonitor[i]);
        screen_x = mode->width;
        screen_y = mode->height;
        //std::cout << "Screen size is X = " << screen_x << ", Y = " << screen_y << std::endl;
        printf("Screen size is X = %d , Y = %d\r\n", screen_x, screen_y);
        if (screen_x == 1920 && screen_y == 1080) {
            holographic_screen = i;
        }
    }
    //std::cout << holographic_screen << std::endl;
    printf("%d\r\n",holographic_screen);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);   //没有边框和标题栏
    
    //GLFWwindow* window = glfwCreateWindow(screen_x, screen_y, "Dear ImGui GLFW+OpenGL3 example", pMonitor[holographic_screen], NULL);
    GLFWwindow* window = glfwCreateWindow(screen_x, screen_y, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);

    HWND hwnd = GetActiveWindow();
    HWND hwndglfw = glfwGetWin32Window(window);

    
    //DWM_BLURBEHIND bb = { 0 };
    //HRGN hRgn = CreateRectRgn(0, 0, -1, -1); //应用毛玻璃的矩形范围，
    ////参数(0,0,-1,-1)可以让整个窗口客户区变成透明的，而鼠标是可以捕获到透明的区域
    //bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    //bb.hRgnBlur = hRgn;
    //bb.fEnable = TRUE;
    //DwmEnableBlurBehindWindow(hwnd1, &bb);

    /*MARGINS margins = { -1, -1, -1, -1 };
    DWM_BLURBEHIND bb = { 0 };
    DwmExtendFrameIntoClientArea(hwnd1, &margins);
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.fEnable = true;
    bb.hRgnBlur = NULL;
    DwmEnableBlurBehindWindow(hwnd1, &bb);*/

    //verify(SetLayeredWindowAttributes(hWnd, 0x0, 0, LWA_COLORKEY));
    //SetLayeredWindowAttributes(::FindWindow(NULL, "Dear ImGui GLFW+OpenGL3 example"), 0x0, 0, LWA_COLORKEY);
    //GetActiveWindow();
    //SetLayeredWindowAttributes(GetActiveWindow(), RGB(0, 0, 0), NULL, LWA_COLORKEY);
    //SetLayeredWindowAttributes(FindWindow(NULL, "Dear ImGui GLFW+OpenGL3 example"), RGB(0, 0, 0), NULL, LWA_COLORKEY);

    //SetWindowLong(GetActiveWindow(), GWL_EXSTYLE, WS_EX_LAYERED);
    //SetLayeredWindowAttributes(GetActiveWindow(), 0, 240, LWA_ALPHA);//alpha = 240，范围0~255；也可以使用关键色，详见MSDN


    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    SetWindowPos(hwndglfw, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

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
    bool show_user_demo = false;
    bool show_random_background = false;
    bool show_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    char* title = new char[30];
    //Shader shader("../src/vShaderSource.vs", "../src/fShaderSource.fs");
    Shader shader("./src/vShaderSource.vs", "./src/fShaderSource.fs");
    shader.initArgument("./src/background.jpg");

    //SequenceFrame sequenceFrame;
    //sequenceFrame.Init("imageFrame/2", "", 50);

    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    //// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.


    int imgnum = 0;
    
    unsigned int texture[ImgTotalnum];

    char imgFileName[256];
    //for (int i = 0; i < ImgTotalnum; ++i)
    //{
    //    memset(imgFileName, 0, 256);
    //    //sprintf(imgFileName,"%s/%s%d.png", filePath, fileHeadName, i+1);
    //    sprintf(imgFileName, "%s/%s%08d.jpg", "imageFrame/3", "", i + 1);
    //    texture[i] = loadtexture(imgFileName,true);
    //    printf("%s\n", imgFileName);
    //}

    //texture1 = loadtexture("src/Microsoft_WindowsInsiderProgram_Wallpaper.png");
    //texture2 = loadtexture("src/background.jpg");            
    int f_num = 0;
    

    bool exitbutton = true;

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));


    if (exechandle1() == -1)
    {
        return -1;
    }


    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();



        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ////ImGui::Begin("Hello, world!",&exitbutton);                          // Create a window called "Hello, world!" and append into it.
            glfwSetWindowShouldClose(window, !exitbutton);
            //char* title = ("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            //sprintf(title, "%.1f FPS", ImGui::GetIO().Framerate);
            //char title = (char)ImGui::GetIO().Framerate;
            //printf(("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate), title);
            //ImGui::Begin(title);

            ////ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ////ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ////ImGui::Checkbox("Another Window", &show_another_window);
            ////ImGui::Checkbox("user demo", &show_user_demo);
            ////ImGui::Checkbox("random background", &show_random_background);


            ////ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ////ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            ////if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            ////    counter++;
            ////ImGui::SameLine();
            ////ImGui::Text("counter = %d", counter);
            ////if (ImGui::Button("Close Me"))
            ////    glfwSetWindowShouldClose(window, true);


            currentTime = GetTickCount() * 0.001f;
            ///*if (ImGui::Button("Hide"))
            //{
            //    ShowWindow(hwnd1, SW_HIDE);
            //                    
            //}*/
            if (int(currentTime - lastTime) > BACKTIME)
            {
                handle1 = FindWindow(NULL, szAppWindowName);
                if (handle1 != NULL)
                {

                }
                else
                {
                    if (exechandle1() == -1)
                    {
                        cout << "glfwSetWindowShouldClose" << endl;
                        glfwSetWindowShouldClose(window, true);
                        //return -1;
                    }
                    
                }
                ShowWindow(handle1, SW_HIDE);
                /*hWndPopup = GetLastActivePopup(hwndglfw);
                BringWindowToTop(hwndglfw);
                if (IsIconic(hWndPopup))
                    ShowWindow(hwndglfw, SW_RESTORE);
                else
                    SetForegroundWindow(hWndPopup);  */                  
                ShowWindow(hwndglfw, SW_SHOW);
                //ShowWindow(hwndglfw, SW_RESTORE);
                SetForegroundWindow(hwndglfw);
                lastTime = GetTickCount() * 0.001f;
                printf("bring glfw to front.\r\n");
            }

            ImGuiIO& io = ImGui::GetIO();
            //ImGui::Text("Mouse clicked:");
            for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
            {
                if (ImGui::IsMouseClicked(i))
                {
                    ImGui::SameLine();
                    ImGui::Text("b%d", i);
                    ShowWindow(hwndglfw, SW_HIDE);
                    /*hWndPopup1 = GetLastActivePopup(handle1);
                    BringWindowToTop(handle1);
                    if (IsIconic(hWndPopup1))
                        ShowWindow(handle1, SW_RESTORE);
                    else
                        SetForegroundWindow(hWndPopup1);*/
                    //ShowWindow(handle1, SW_SHOW);
                    handle1 = FindWindow(NULL, szAppWindowName);
                    if (handle1 != NULL)
                    {

                    }
                    else
                    {
                        if (exechandle1() == -1)
                        {
                            glfwSetWindowShouldClose(window, true);
                            //return -1;
                        }
                    }
                    //ShowWindow(handle1, SW_RESTORE);
                    ShowWindow(handle1, SW_SHOW);
                    SetForegroundWindow(handle1);
                    printf("bring handle1 to front.\r\n");
                }
            }
                

            ////ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ////ImGui::End();
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

        // 自定义窗口
        if (show_user_demo)
        {
            ImGuiIO& io = ImGui::GetIO();
            //设置窗口位置
            ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
            //设置窗口的大小
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
            //设置窗口为透明
            ImGui::SetNextWindowBgAlpha(0);
            //纹理ID
            static ImTextureID bg_tex_id = 0;
            if (!bg_tex_id)
            {
                //这里使用opencv加载图片当然你也可以使用其他方式加载图片
                //loadTexture是一个自定义的函数用于图片字符加载为纹理，不知道怎么加载纹理的可以自行百度
                //bg_tex_id = (GLuint*)loadTexture(cvLoadImage("Microsoft_WindowsInsiderProgram_Wallpaper.png"));
                //stbi_set_flip_vertically_on_load(true);
                bg_tex_id = (GLuint*)loadtexture("src/Microsoft_WindowsInsiderProgram_Wallpaper.png", false);
            }
            //设置窗口的padding为0是图片控件充满窗口
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            //设置窗口为无边框
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
            //创建窗口使其固定在一个位置
            ImGui::Begin("background", NULL, ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoScrollbar);
            ImGui::Image(bg_tex_id, ImGui::GetContentRegionAvail());
            ImGui::End();
            ImGui::PopStyleVar(2);
        }



        if (show_random_background && (imgnum != 1))
        {
            //shader.initArgument("./src/Microsoft_WindowsInsiderProgram_Wallpaper.png");
            //shader.use();
            //int width, height, nrChannels;
            //unsigned char* data = stbi_load("./src/Microsoft_WindowsInsiderProgram_Wallpaper.png", &width, &height, &nrChannels, 0);
            //if (data)
            //{
            //    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            //    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            //    //glGenerateMipmap(GL_TEXTURE_2D);
            //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            //}
            //else
            //{
            //    //std::cout << "Failed to load texture" << std::endl;
            //    printf("Failed to load texture1\r\n");
            //}
            ////glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            //stbi_image_free(data);
            //glBindTexture(GL_TEXTURE_2D, texture1);
            //imgnum = 1;
            currentTime = GetTickCount() * 0.001f;
            if ((currentTime - lastTime) > double(1 * 0.001f / (30 * 0.001f)))
            {
                glBindTexture(GL_TEXTURE_2D, texture[f_num]);
                if (f_num < ImgTotalnum)
                    f_num++;
                else
                    f_num = 0;
                printf("%d  %f\r\n", f_num, currentTime - lastTime);
                lastTime = GetTickCount() * 0.001f;
            }
        }
        else if ((!show_random_background) && (imgnum != 2))
        {
            //int width1, height1, nrChannels1;
            //unsigned char* data1 = stbi_load("./src/background.jpg", &width1, &height1, &nrChannels1, 0);
            //if (data1)
            //{
            //    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            //    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            //    //glGenerateMipmap(GL_TEXTURE_2D);
            //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);                
            //}
            //else
            //{
            //    //std::cout << "Failed to load texture" << std::endl;
            //    printf("Failed to load texture2\r\n");
            //}
            ////glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            //stbi_image_free(data1);
            //glBindTexture(GL_TEXTURE_2D, texture2);
            //imgnum = 2;
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glEnable(GL_BLEND);
        //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        //sequenceFrame.drawRect(0.0,0.0,1.0,1.0,1.0);
        //sequenceFrame.update();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    string process_name1 = PROCCESS_NAME;
    size_t size = process_name1.length();
    wchar_t* buffer1 = new wchar_t[size + 1];
    MultiByteToWideChar(CP_ACP, 0, process_name1.c_str(), size, buffer1, size * sizeof(wchar_t));
    buffer1[size] = 0;  //确保以 '\0' 结尾 
                       //      ::MessageBox(NULL, buffer, NULL, NULL);
    TerminateProcessByName(buffer1);
    delete buffer1;
    buffer1 = NULL;

    return 0;
}

// Win32 message handler
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
//        return true;
//
//    switch (msg)
//    {
//    case WM_SIZE:
//        /*if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
//        {
//            g_d3dpp.BackBufferWidth = LOWORD(lParam);
//            g_d3dpp.BackBufferHeight = HIWORD(lParam);
//            ResetDevice();
//        }*/
//        return 0;
//    case WM_SYSCOMMAND:
//        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
//            return 0;
//        break;
//    case WM_DESTROY:
//        ::PostQuitMessage(0);
//        return 0;
//    }
//    return ::DefWindowProc(hWnd, msg, wParam, lParam);
//}
