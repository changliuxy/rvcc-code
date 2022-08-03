# RVCC
本项目为[徒手写一个RISC-V编译器](https://www.bilibili.com/video/BV1gY4y1E7Ue)代码

## 代码
先设置环境变量，自动补全`~/riscv/bin/`下的命令
`export PATH=~/riscv/bin:$PATH`
编译代码
```
riscv64-unknown-linux-gnu-gcc -static tmp.s -o tmp
qemu-riscv64 -L $RISCV/sysroot tmp
```
[第001课 - 编译出能返回指定数值的程序](src/001-main.c)

[第002课 - 支持+ -运算符](src/002-main.c)s