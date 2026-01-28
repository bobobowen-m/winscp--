#include "RDPConfig.h"
#include <sstream>
#include <algorithm>

// 根据预设获取分辨率字符串（供外部调用/日志使用）
std::wstring RDPConfig::GetResolutionString() const {
    switch (resPreset) {
        case ResolutionPreset::FullScreen: return L"Full Screen";
        case ResolutionPreset::AutoFit:    return L"Auto Fit";
        case ResolutionPreset::HD:         return L"1280x720";
        case ResolutionPreset::FHD:        return L"1920x1080";
        case ResolutionPreset::QHD:        return L"2560x1440";
        case ResolutionPreset::UHD:        return L"3840x2160";
        case ResolutionPreset::Custom:     return customRes.empty() ? L"Auto Fit" : customRes;
        default:                           return L"Auto Fit";
    }
}

// 核心：拼接 MSTSC 命令行参数（分辨率逻辑是重点）
std::wstring RDPConfig::ToMSTSCArgs() const {
    std::wostringstream args;
    
    // 基础连接参数
    args << L" /v:" << host << L":" << port;
    
    // 管理员会话
    if (adminSession) {
        args << L" /admin";
    }

    // ========== 分辨率参数处理（核心逻辑）==========
    switch (resPreset) {
        case ResolutionPreset::FullScreen:
            // MSTSC /f 表示全屏模式
            args << L" /f";
            break;
        
        case ResolutionPreset::Custom:
            // 自定义分辨率：解析 WxH 格式，拼接 /w (宽度) /h (高度)
            if (!customRes.empty()) {
                size_t xPos = customRes.find(L"x");
                // 校验格式：必须包含x，且x不在开头/结尾
                if (xPos == std::wstring::npos || xPos == 0 || xPos == customRes.length()-1) {
                    throw std::invalid_argument("Invalid custom resolution format (expected WxH, e.g. 1920x1080)");
                }
                // 校验宽高为数字
                std::wstring widthStr = customRes.substr(0, xPos);
                std::wstring heightStr = customRes.substr(xPos + 1);
                if (!std::all_of(widthStr.begin(), widthStr.end(), iswdigit) ||
                    !std::all_of(heightStr.begin(), heightStr.end(), iswdigit)) {
                    throw std::invalid_argument("Resolution width/height must be numeric");
                }
                args << L" /w:" << widthStr << L" /h:" << heightStr;
            }
            // 自定义分辨率为空则用 MSTSC 默认（自适应）
            break;
        
        case ResolutionPreset::HD:
            args << L" /w:1280 /h:720";
            break;
        
        case ResolutionPreset::FHD:
            args << L" /w:1920 /h:1080";
            break;
        
        case ResolutionPreset::QHD:
            args << L" /w:2560 /h:1440";
            break;
        
        case ResolutionPreset::UHD:
            args << L" /w:3840 /h:2160";
            break;
        
        case ResolutionPreset::AutoFit:
        default:
            // 自适应窗口（MSTSC 默认行为，无需加分辨率参数）
            break;
    }

    // 颜色深度（MSTSC /bpp 参数）
    if (!colorDepth.empty()) {
        // 校验颜色深度合法性
        if (colorDepth != L"8" && colorDepth != L"16" && colorDepth != L"24" && colorDepth != L"32") {
            throw std::invalid_argument("Color depth must be 8/16/24/32");
        }
        args << L" /bpp:" << colorDepth;
    }

    return args.str();
}
