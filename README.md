<div align="center">

# Aristotle

Aristotle is an IDLE like environment for [ndpc](https://xiaoshihou514.github.io/ndpc)

</div>

## Demo

Click to watch video demo:

[![Watch the demo](https://github.com/user-attachments/assets/6d832f61-1169-4076-aaf7-6901561ebf9f)](https://github.com/user-attachments/assets/bc463b97-6809-4e31-9acd-d0dcb89867b8)

## Installation

The easiest way is download the prebuilt releases.

To install from source:

- Get the prerequisites:
  - Linux:
    ```bash
    sudo apt-get install git make libglfw-dev g++
    sudo dnf install git make glfw-devel gcc-c++
    ```
  - Macos:
    ```bash
    brew install git make glfw
    # You need to agree to clang++'s user terms
    ```
  - Windows
    - Install [msys2](https://www.msys2.org/)
    ```bash
    pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw git make
    ```
    - Run the build command in mingw64 window.
- Build

```bash
git clone https://github.com/xiaoshihou514/aristotle --recurse-submodules
make sync
make
```

## Troubleshooting

The app is currently in beta, feel free to open an issue if anything doesn't work :)
