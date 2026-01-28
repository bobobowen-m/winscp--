#ifndef RDP_CONFIG_H
#define RDP_CONFIG_H

#include <Windows.h>
#include <string>
#include <stdexcept>

// 分辨率预设类型（覆盖常见使用场景）
enum class ResolutionPreset {
    Custom,     // 自定义分辨率
    FullScreen, // 全屏（优先）
    AutoFit,    // 自适应窗口（MSTSC 默认）
    HD,         // 1280x720
    FHD,        // 1920x1080
    QHD,        // 2560x1440
    UHD         // 3840x2160
};

// RDP 会话配置（强化分辨率逻辑）
struct RDPConfig {
    std::wstring host;          // 远程主机IP/域名
    UINT16 port = 3389;         // RDP默认端口
    std::wstring username;      // 登录用户名
    std::wstring password;      // 登录密码（可选）
    ResolutionPreset resPreset = ResolutionPreset::AutoFit; // 分辨率预设
    std::wstring customRes;     // 自定义分辨率（仅 resPreset=Custom 时生效，格式：WxH）
    std::wstring colorDepth;    // 颜色深度（"8"/"16"/"24"/"32"）
    bool adminSession = false;  // 是否管理员会话

    // 转换为 MSTSC 命令行参数（重点优化分辨率）
    std::wstring ToMSTSCArgs() const;
    
    // 辅助：根据预设获取分辨率字符串（如 FHD → "1920x1080"）
    std::wstring GetResolutionString() const;
};

#endif // RDP_CONFIG_H
