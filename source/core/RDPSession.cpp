#include "RDPSession.h"
#include "RDPConfig.h"
#include <Windows.h>
#include <shellapi.h>
#include <iostream>
#include <fstream>

// 启动 RDP 会话（包含分辨率参数）
bool StartRDPSession(const RDPConfig& config) {
    // MSTSC 路径（Windows 系统默认路径，兼容32/64位）
    std::wstring mstscPath = L"C:\\Windows\\System32\\mstsc.exe";
    
    // 生成命令行参数（会自动处理分辨率）
    std::wstring cmdArgs;
    try {
        cmdArgs = config.ToMSTSCArgs();
    } catch (const std::exception& e) {
        // 捕获分辨率格式错误等异常
        std::cerr << "[RDP Error] Invalid config: " << e.what() << std::endl;
        return false;
    }

    // 调试用：打印最终执行的命令（可根据需要删除）
    std::wcout << L"[RDP Debug] Execute: " << mstscPath << L" " << cmdArgs << std::endl;

    // 构造 SHELLEXECUTEINFO 结构体启动进程
    SHELLEXECUTEINFOW sei = { 0 };
    sei.cbSize = sizeof(SHELLEXECUTEINFOW);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpFile = mstscPath.c_str();
    sei.lpParameters = cmdArgs.c_str();
    sei.nShow = SW_SHOWNORMAL; // 正常显示窗口

    if (!ShellExecuteExW(&sei)) {
        DWORD err = GetLastError();
        std::cerr << "[RDP Error] Failed to start MSTSC, code: " << err << std::endl;
        return false;
    }

    // 可选：等待进程退出（如需阻塞当前线程，注释打开）
    // WaitForSingleObject(sei.hProcess, INFINITE);
    // CloseHandle(sei.hProcess);

    std::cout << "[RDP Info] Session started to " << config.host << " (Resolution: " 
              << config.GetResolutionString() << ")" << std::endl;
    return true;
}

// 保存 RDP 配置文件（.rdp），支持后续复用/手动编辑
bool SaveRDPFile(const RDPConfig& config, const std::wstring& filePath) {
    std::wstring rdpContent;
    // 基础配置
    rdpContent += L"full address:s:" + config.host + L":" + std::to_wstring(config.port) + L"\r\n";
    rdpContent += L"username:s:" + config.username + L"\r\n";
    // 分辨率配置
    if (config.resPreset == ResolutionPreset::FullScreen) {
        rdpContent += L"screen mode id:i:1\r\n"; // 1=全屏
    } else if (config.resPreset != ResolutionPreset::AutoFit) {
        rdpContent += L"screen mode id:i:2\r\n"; // 2=窗口模式
        std::wstring res = config.GetResolutionString();
        if (res != L"Auto Fit") {
            size_t xPos = res.find(L"x");
            if (xPos != std::wstring::npos) {
                rdpContent += L"desktopwidth:i:" + res.substr(0, xPos) + L"\r\n";
                rdpContent += L"desktopheight:i:" + res.substr(xPos + 1) + L"\r\n";
            }
        }
    }
    // 颜色深度
    if (!config.colorDepth.empty()) {
        rdpContent += L"session bpp:i:" + config.colorDepth + L"\r\n";
    }
    // 管理员会话
    if (config.adminSession) {
        rdpContent += L"administrative session:i:1\r\n";
    }

    // 写入文件
    std::wofstream rdpFile(filePath);
    if (!rdpFile.is_open()) {
        std::cerr << "[RDP Error] Failed to create RDP file: " << filePath << std::endl;
        return false;
    }
    rdpFile << rdpContent;
    rdpFile.close();

    std::cout << "[RDP Info] RDP file saved to: " << filePath << std::endl;
    return true;
}

// 【测试示例】调用不同分辨率的 RDP 会话
void TestRDPSessionWithResolution() {
    // 示例1：全屏模式
    RDPConfig fullScreenConfig;
    fullScreenConfig.host = L"192.168.1.100";
    fullScreenConfig.resPreset = ResolutionPreset::FullScreen;
    StartRDPSession(fullScreenConfig);

    // 示例2：预设 FHD (1920x1080)
    RDPConfig fhdConfig;
    fhdConfig.host = L"192.168.1.100";
    fhdConfig.resPreset = ResolutionPreset::FHD;
    fhdConfig.colorDepth = L"32"; // 32位色深
    StartRDPSession(fhdConfig);

    // 示例3：自定义分辨率（1440x900）
    RDPConfig customResConfig;
    customResConfig.host = L"192.168.1.100";
    customResConfig.resPreset = ResolutionPreset::Custom;
    customResConfig.customRes = L"1440x900";
    StartRDPSession(customResConfig);

    // 示例4：保存 RDP 文件（后续可双击打开）
    SaveRDPFile(fhdConfig, L"C:\\temp\\winscp_rdp_fhd.rdp");
}
