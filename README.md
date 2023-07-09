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

> 以上内容库中都已经make安装好，可以进入lib中查看

要运行AF_XDP代码，需要先打开测试环境

到testenv环境下

```bash
$ eval $(../testenv/testenv.sh alias)
$ t setup --name veth-adv03
$ t ping
```

创建veth-adv03测试网络接口，同时不断ping虚拟机

到AF_XDP文件夹下make生成可执行文件./af_xdp_user

```bash
$ sudo ./af_xdp_user -d veth-adv03 --filename af_xdp_kern.o
```

可以将指定af_xdp_kern.o挂载到veth-adv03上

会输出xdp程序的收发包结果