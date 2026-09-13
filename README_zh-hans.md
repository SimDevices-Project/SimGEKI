# SimGETRO 固件 (CH32V203版本)

[简体中文](./README_zh-hans.md)

本仓库为基于CH32V203芯片驱动的SimGETRO固件

## 协议

除非文件或目录另有说明，本仓库中由贡献者持有权利的部分以
[PolyForm Noncommercial License 1.0.0](./LICENSE) 作为源码可用许可发布。
该许可允许以非商业目的使用、修改和分发；其条款不授予商业使用权。商业使用必须事先
取得项目所有者的独立书面授权，请通过
[SimGEKI 仓库](https://github.com/iamapig120/SimGEKI)联系项目所有者。

PolyForm Noncommercial 并非 OSI 认可的开源许可证，因此本项目应表述为“源码可用”，
而非“开源”。`sdk/`、`tools/` 等第三方组件继续适用其各自的许可声明。

## 编译

要编译源代码，请在您的 VSCode 中安装 EIDE 作为开发编译环境，并使用 EIDE 安装 RISCV_GCC 和 OPENOCD_WCH 8.2.0 ，如果您使用其他版本的 openocd ，请修改 EIDE项目 中的 构建配置->右键菜单 修改您的工具链路径。请注意，更改工具链版本可能会造成无法编译。

要烧入固件，请使用 WCHISPTools，并使用 USB 方式烧入。

## 文件

您可以根据以下表格找到您可能需要的文件

| 文件夹 | 内容                      |
| ------ | ------------------------- |
| src    | 固件源代码                |
| sdk    | CH32V SDK                 |
| ref    | 参考资料，如立创EDA源文件 |
| tools  | Windows工具链             |

## 自己造一台！

[SimDevices Wiki 页面](https://sim.bysb.net/#/simgetro/diy/core_and_firmware/)
查看该页面来了解如果拥有一台属于你自己的SimGEKi，内容包含但不限于原理图、PCB设计甚至还包含CAD设计图！
Check this page to get your own SimGEKI, Sch files, Pcb files even Cad files all included!

## 商标

软件许可不授予 SimPad、SimDevices、SimGEKI、SimGETRO 名称或商标的使用权。未经事先
许可，请勿使用这些名称宣传产品，也不得暗示获得项目方赞助、认可或与项目方存在关联。

## 特别感谢

这个项目参考了以下项目，在此表示感谢。

- [ONGEEKi Firmware](https://github.com/GEEKiDoS/ongeeki-firmware) - GEEKiDoS
- [Arduino-Aime-Reader](https://github.com/Sucareto/Arduino-Aime-Reader) - Sucareto
- [Arduino-Aime-QReader](https://github.com/QHPaeek/Arduino-Aime-QReader) - QHPaeek
