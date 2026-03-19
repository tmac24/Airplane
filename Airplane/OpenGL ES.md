## OpenGL ES 跨平台支持概览

| 平台                           | 原生 OpenGL ES | 说明                                                         | 需要导入源码/库吗          |
| ------------------------------ | -------------- | ------------------------------------------------------------ | -------------------------- |
| **iOS (iPhone/iPad)**          | ✅ ES 2/3       | 系统自带框架 `<OpenGLES/ES2/gl.h>` / `<OpenGLES/ES3/gl3.h>`  | 不需要                     |
| **Android**                    | ✅ ES 2/3/3.1   | GPU 驱动提供，NDK 提供头文件                                 | 不需要                     |
| **Windows**                    | ❌              | 没有原生 ES，桌面 OpenGL 可以用 ANGLE 或 Mesa 模拟 ES        | 需要 ANGLE 或 Mesa         |
| **Linux**                      | ❌              | GPU 驱动可能提供桌面 OpenGL，可用 Mesa/EGL 实现 ES           | 需要 Mesa 或 EGL           |
| **macOS**                      | ❌（弃用）      | 系统 OpenGL 支持较旧，推荐 Metal；ES 可以通过 ANGLE 转桌面 GL | 需要 ANGLE                 |
| **Web**                        | ❌              | WebGL 是基于 OpenGL ES 2/3 子集                              | 不需要导入源码，浏览器封装 |
| **游戏主机 (PS/Xbox/Switch)**  | ❌              | 自己封装图形 API 或厂商 SDK                                  | 需要厂商 SDK               |
| **嵌入式 / 单片机 / 自研设备** | ❌              | 没有 GPU 驱动，通常用 TinyGL / Mesa                          | 需要导入源码或交叉编译     |

## OpenGL 本质

- OpenGL 是一个 **图形 API**，提供 GPU 渲染接口
- 它的规范（OpenGL Specification）定义了：
  - 函数、常量、行为
  - 渲染管线（顶点处理、光栅化、片段着色器等）
- **实际实现**由 GPU 驱动提供：
  - AMD/NVIDIA/Intel/Apple 的驱动在后台实现这些函数
  - 你调用的 `glDrawArrays`、`glBindBuffer` 等都是驱动的封装