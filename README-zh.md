<div align="center">

# Aristotle

<img src="https://github.com/user-attachments/assets/5afd0f80-14a0-476e-9f28-4b6233c29e4e" alt="logo" width="30%" />

Aristotle是基于[ndpc](https://xiaoshihou514.github.io/ndpc)的集成开发环境

</div>

## 演示

[![演示](https://github.com/user-attachments/assets/6d832f61-1169-4076-aaf7-6901561ebf9f)](https://github.com/user-attachments/assets/bc463b97-6809-4e31-9acd-d0dcb89867b8)

## 安装

推荐直接下载[预编译版本](https://github.com/xiaoshihou514/aristotle/releases)。

编译安装：

- 安装编译依赖：
  - Linux系统：
    ```bash
    # Debian/Ubuntu
    sudo apt-get install git make libglfw-dev g++
    # Fedora
    sudo dnf install git make glfw-devel gcc-c++
    ```
  - macOS系统：
    ```bash
    brew install git make glfw
    # 请自行同意clang++的用户协议
    ```
  - Windows系统：
    - 安装 [msys2](https://www.msys2.org/)
    ```bash
    pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw git make
    ```
    - 在 mingw64 窗口中运行构建命令
- 执行构建：

```bash
git clone https://github.com/xiaoshihou514/aristotle --recurse-submodules
make sync
make
```

## 问题反馈

应用目前处于测试阶段，若遇到任何问题欢迎提交工单 :)
