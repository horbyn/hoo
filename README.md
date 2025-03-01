## hoo

![](https://pic1.imgdb.cn/item/67c11732d0e0a243d4078e8f.gif)

## 开发环境

使用模拟器来虚拟化硬件环境，模拟器使用两个：

- [Qemu](https://www.qemu.org/)：可以模拟多种架构，好处是可以对接 `gdb`，调试起来非常方便。缺点是调试依赖于符号表，有符号表的地方再结合图形化界面进行调试就非常方便。但符号表覆盖不到的地方，比如没有标号导出的汇编指令，调试起来略嫌麻烦。另外 `gdb` 对于控制寄存器、物理地址观察等方便支持也不太好
- [Bochs](https://bochs.sourceforge.io/)：它只能模拟 x86 架构，好处是可以精确到每条汇编指令的执行。在内核的实现中除了内核本身，还包括一些内置命令（`ls`、`cat`）等的开发，这些命令加载到内存后，没办法在已加载内核二进制符号表的情况下加载其它二进制文件的符号表，意味着调试这些命令时 `gdb` 图形化调试的优势已经不存在了。另一方面，`bochs` 对控制寄存器、物理地址输出等支持比较好

除此之外还需要一个 X Server 将虚拟机中的界面转发到宿主环境
- Windows：[Xming](https://sourceforge.net/projects/xming/files/Xming/6.9.0.31/#:~:text=Xming%2D6%2D9%2D0%2D31%2Dsetup.exe)，其他软件也可以，只是我只用过这个（Windows 11 23H2 以上不需要）
- MacOS：[XQuartz](https://github.com/XQuartz/XQuartz/releases/download/XQuartz-2.8.5/XQuartz-2.8.5.pkg)

### Docker 部署（推荐）

执行

```shell
docker buildx build --platform linux/amd64 --load -t kernx86:latest .

# /path/to 改成要映射的目录
docker run --platform linux/amd64 -it --privileged -v /path/to/:/path/to/ --name kern kernx86:latest /bin/bash
```

### 手动部署

可以参考 `Dockerfile`

## Build

```shell
# 编译
make debug -j 8

# 格式化磁盘
make format_disk

# 用 bochs 来运行
make bochs
```

## vscode 图形化调试

Visual Studio Code 安装 `C/C++` 插件，安装完成后点击「运行」菜单，点击「添加配置」，保存以下配置文件：

```json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "debug",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/kernel.elf",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${fileDirname}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "/usr/bin/gdb_sudo",
            "miDebuggerServerAddress": "localhost:12345",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "Set Disassembly Flavor to AT&T",
                    "text": "-gdb-set disassembly-flavor ATT",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

- `"miDebuggerPath": "/usr/bin/gdb_sudo"`：`Dockerfile` 创建了 gdb_sudo 文件，主要用来让 `vscode` 在调试时使用 sudo 权限。而直接用 `"miDebuggerPath": "/usr/bin/gdb"` 或不用这个 key 可能调试时会报错
- `"miDebuggerServerAddress": "localhost:12345"`：调试时需要先在一个终端窗口启动 `qemu`（`bochs` 不行），具体命令见下文，然后再点击 F5

启动调试版 `qemu` 用以下命令：

```shell
make qemu-debug
```

## 已知问题

- macOS M2 芯片（其他 M 系列芯片不清楚）在执行 `x86` 架构需要转译，虽然容器环境已经通过 `--platform linux/amd64` 确定好了，但是后面用 vscode 调试时会经常崩溃，而在宿主环境为 `x86` 的容器上没有这种现象
- qemu 由于不知道键盘输入和 80*25 显示输出在启动时要带什么选项，所以目前无法启动。但是用来配合 gdb 调试还是可以的
