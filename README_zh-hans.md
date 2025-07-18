# SimGETRO 固件 (CH32V203版本)

![License](https://img.shields.io/github/license/SimDevices-Project/SimGEKI.svg)

[简体中文](./README_zh-hans.md)

本仓库为基于CH32V203芯片驱动的SimGETRO固件

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

## 注意

请您知晓，您不得将与SimPad、SimDevices、SimGEKI、SimGETRO相关的任何名称（包括产品名称和商标）用于销售或其他目的。

## 特别感谢

这个项目参考了以下项目，在此表示感谢。

- [ONGEEKi Firmware](https://github.com/GEEKiDoS/ongeeki-firmware) - GEEKiDoS
- [Arduino-Aime-Reader](https://github.com/Sucareto/Arduino-Aime-Reader) - Sucareto
- [Arduino-Aime-QReader](https://github.com/QHPaeek/Arduino-Aime-QReader) - QHPaeek