<p align="center"><img width="300" src="https://avatars.githubusercontent.com/u/423027?s=200&v=4" alt="XFEL logo"></p>
<h1 align="center">XFEL</h1>
<p align="center">Tiny FEL tools for Allwinner SOC.</p>

## 关于 XFEL

面向全志 SOC 的 FEL 工具。

## 什么是FEL
FEL 是全志 SOC 中上 BootROM 中包含的低级程序。 可以通过它使用 USB  OTG 对 SOC 进行编程和恢复。

## 支持列表

✅: 已经支持 — ❌: 还未支持  — ⚠️: 还未完全支持

| 芯片名称 |  CPU  |  ID  |  状态  |  备注  |
| -------- | :----- | :----- | -------- | :------- |
| Allwinner A10 | ARM Cortex-A8 @ 1Ghz | 0x00162300 |   ❌   |      |
| Allwinner A10s | ARM Cortex-A8 @ 1Ghz | 0x00162500 |   ❌   |      |
| Allwinner A13 | ARM Cortex-A8 @ 1Ghz | 0x00162500 |   ❌   |      |
| Allwinner R8 | ARM Cortex-A8 @ 1Ghz | 0x00162500 | ❌ |  |
| Allwinner A20 | Dual-Core ARM Cortex-A7 | 0x00165100 | ❌ |  |
| Allwinner A23 | Dual-Core ARM Cortex-A7 @ 1.5GHz | 0x00165000 | ❌ |  |
| Allwinner A31 | Quad-Core ARM Cortex-A7 | 0x00163300 | ❌ |  |
| Allwinner A33 | Quad-Core ARM Cortex-A7 @ 1.2GHz | 0x00166700 | ❌ |  |
| Allwinner R16 | Quad-Core ARM Cortex-A7 @ 1.2GHz | 0x00166700 | ❌ |  |
| Allwinner A40i | Quad-Core ARM Cortex-A7 @ 1.2 GHz | 0x00170100 | ❌ |  |
| Allwinner R40 | Quad-Core ARM Cortex-A7 @ 1.2 GHz | 0x00170100 | ❌ |  |
| Allwinner A64 | Quad-Core ARM Cortex-A53 | 0x00168900 | ❌ |  |
| Allwinner A80 | Octa-core big.LITTLE Cortex-A15/7 | 0x00163900 | ❌ |  |
| Allwinner A83T | Octa-core Cortex-A7 @ 1.6GHz | 0x00167300 | ❌ |  |
| Allwinner D1 | XuanTie C906 RISC-V | 0x00185900 | ✅ |  |
| Allwinner F133 (D1s) | XuanTie C906 RISC-V | 0x00185900 | ✅ |  |
| Allwinner F1C100S | Single-core ARM9 | 0x00166300 | ✅ |  |
| Allwinner F1C200S | Single-core ARM9 | 0x00166300 | ✅ |  |
| Allwinner F1C500S | Single-core ARM9 | 0x00166300 | ✅ |  |
| Allwinner H2 | Quad-Core ARM Cortex-A7 | 0x00168000 | ✅ |  |
| Allwinner H3 | Quad-Core ARM Cortex-A7 @ 1.296GHz | 0x00168000 | ✅ |  |
| Allwinner H5 | Quad-Core ARM Cortex-A53 | 0x00171800 | ❌ |  |
| Allwinner H6 | Quad-Core ARM Cortex-A53 @ 1.8GHz | 0x00172800 | ❌ |  |
| Allwinner H616 | Quad-Core ARM Cortex-A53 | 0x00182300 | ❌ |  |
| Allwinner R328 | Dual-Core Cortex-A7 | 0x00182100 | ⚠️ | Only chip_sid |
| Allwinner R329 | Dual-Core Cortex-A53 | 0x00185100 | ❌ |  |
| Allwinner T507 | Quad-Core ARM Cortex-A53 | 0x00182300 | ❌ |  |
| Allwinner V3s | Single-core Cortex-A7 | 0x00168100 | ✅ |  |
| Allwinner S3 | Single-core Cortex-A7 | 0x00168100 | ✅ |  |
| Allwinner V536 | Dual-Core Cortex-A7 | 0x00181600 | ❌ |  |
| Allwinner V831 | Single-core Cortex-A7 800Mhz | 0x00181700 | ✅ |  |

## 使用方法

```
usage:
    xfel version                                        - 显示芯片ID
    xfel hexdump <address> <length>                     - 以HEX文件转储内存区域
    xfel dump <address> <length>                        - 转储内存区域并输出
    xfel exec <address>                                 - 调用函数地址
    xfel read32 <address>                               - 从设备内存中读取 32 位值
    xfel write32 <address> <value>                      - 向设备内存中写入 32 位值
    xfel read <address> <length> <file>                 - 向文件写入内存
    xfel write <address> <file>                         - 向内存写入文件
    xfel reset                                          - 使用看门狗重置设备
    xfel sid                                            - 显示sid信息
    xfel jtag                                           - 启用 jtag 调试
    xfel ddr [type]                                     - 初始化 DDR 控制器
    xfel spinor                                         - 检测 spi nor flash
    xfel spinor read <address> <length> <file>          - 读取 spi nor flash 到文件
    xfel spinor write <address> <file>                  - 将文件写入 spi nor flash
    xfel spinand                                        - 检测 spi nand flash
    xfel spinand read <address> <length> <file>         - 读取 spi nand flash 到文件
    xfel spinand write <address> <file>                 - 将文件写入 spi nand flash
    xfel spinand splwrite <split-size> <address> <file> - 使用拆分支持将文件写入 spi nand flash
```

## 例子

#### F1C100s: 使用XEFL写入 U-Boot 并运行

```
xfel ddr ddr2                          # 使用 ddr2 方式初始化 DDR 控制器
xfel write 0x81700000 u-boot.bin       # 将 uboot 写入 0x81700000
xfel exec 0x81700000                   # 调用函数地址运行
```

#### F133: 使用XEFL写入 OpenSBI 并运行


```
xfel ddr ddr2                          # 使用 ddr2 方式初始化 DDR 控制器
xfel write 0x80200000 opensbi.bin      # 将 opensbi 写入 0x80200000
xfel exec 0x80200000                   # 调用函数地址运行
```

