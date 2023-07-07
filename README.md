# XDP for mainserver

```bash
# 安装依赖项
$ sudo apt install clang llvm libelf-dev libpcap-dev gcc-multilib build-essential
# 注意需要至少clang11+ llvm11+ 注意版本
```

**安装libbpf（clone github 上的库）**

[libbpf/libbpf at b91f53ec5f1aba2a9d01dc00c4434063abd921e8 (github.com)](https://github.com/libbpf/libbpf/tree/b91f53ec5f1aba2a9d01dc00c4434063abd921e8)

```bash
git clone git@github.com:libbpf/libbpf.git
```

**安装bpftool**

[libbpf/bpftool: Automated upstream mirror for bpftool stand-alone build. (github.com)](https://github.com/libbpf/bpftool)

**安装libpcap**

```bash
sudo apt install libpcap-dev
```

**安装xdp-tools（包含loader和libxdp）**

[xdp-project/xdp-tools: Utilities and example programs for use with XDP (github.com)](https://github.com/xdp-project/xdp-tools)

```bash
git clone git@github.com:xdp-project/xdp-tools.git
```

### 1. 无法在XDP程序中打开文件

在XDP程序中，无法使用标准C库中的函数，因为XDP程序运行在内核态中，而标准C库中的函数都是用户态的函数。因此，如果需要在XDP程序中打开文件，需要使用内核态的函数。

### 2. 'asm/types.h' file not found

asm文件夹是老版本内核的头文件，软连接到另一个asm文件夹就好

```bash
cd /usr/include
ln -s x86_64-linux-gnu/asm/ asm
```



加载这段 XDP 程序，您需要使用 `ip` 命令行工具来与内核进行交互，并将 XDP 程序附加到网络接口上。以下是加载 XDP 程序的基本步骤：

1. 使用 clang 编译 XDP 程序，并生成目标文件（例如，`xdp_prog.o`）：

   ```bash
   clang -O2 -target bpf -c xdp_prog.c -o xdp_prog.o
   ```

2. 使用 `ip` 命令加载 XDP 程序到指定的网络接口：

   ```bash
   ip link set dev <interface> xdp obj xdp_prog.o
   ```

   将 `<interface>` 替换为您要加载 XDP 程序的网络接口名称。

3. 确认 XDP 程序是否加载成功：

   ```bash
   ip link show dev <interface>
   ```

   在输出结果中，您应该能够看到接口名称旁边有 `xdp` 字样，表示 XDP 程序已成功加载到该接口。

> ```bash
> ifconfig
> ```
>
> 这会显示当前系统上的网络接口信息，包括接口名称、MAC 地址、IP 地址等，一般会有两个及以上其中lo是本地环回网络接口，en33(数字因人而异)为以太网接口



当 **SYN=1，ACK=0** 时，表示这是一个请求建立连接的报文段；当 SYN=1，ACK=1 时，表示对方同意建立连接。 SYN=1 时，说明这是一个请求建立连接或同意建立连接的报文。 只有在前两次握手中 SYN 才为 1。



我的例程按顺序分以下几个步骤：

创建和配置UMEM、XSK

加载挂载XDP程序到对应的网络接口上（预定义的xsks_map也会在这一步创建）

将我们要绑定的queue_id和得到的xsk_fd更新到xsks_map

收包、处理包、发包

还有就是可以另外创建个线程用于打印收发包状态，我们就忽略了；