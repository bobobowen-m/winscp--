#ifndef RDP_SESSION_H
#define RDP_SESSION_H

#include "RDPConfig.h"
#include <string>

// 启动 RDP 会话（调用 MSTSC）
bool StartRDPSession(const RDPConfig& config);

// 保存 RDP 配置文件（.rdp），支持后续复用
bool SaveRDPFile(const RDPConfig& config, const std::wstring& filePath);

// 测试示例：不同分辨率的 RDP 会话调用
void TestRDPSessionWithResolution();

#endif // RDP_SESSION_H
