# Firmware for SimGETRO (CH32V203 based)

![License AGPL-3.0](https://img.shields.io/github/license/SimDevices-Project/SimGEKI.svg)

The repository for firmware of SimGETRO designed based on the ch32v203 chip.

## Compiling

Currently, you need to install Risc-GCC (WCH) in order to compile the firmware correctly.
Also, you might need VS Code & Embedded IDE plugin as your IDE progrom.
Use WCHISPtool to burn firmware to your device.

要编译源代码，请在您的 VSCode 中安装 EIDE 作为开发编译环境，并使用 EIDE 安装 RISCV_GCC 和 OPENOCD_WCH 8.2.0 ，如果您使用其他版本的 openocd ，请修改 EIDE项目 中的 构建配置->右键菜单 修改您的工具链路径。请注意，更改工具链版本可能会造成无法编译。

要烧入固件，请使用 WCHISPTools，并使用 USB 方式烧入。

## Files

You could refer to the following table to find what you're looking for.

|Folder|Content|
|-|-|
|src|Firmware source code|

## Editing

You could find the schematic information from files under the directory `Schematic Design`. Please add new features to SimGEKI as you prefer! In addition, what you write will have a chance to be pushed to SimGEKI around the world.

> Will upload later

## Attention

You must not use any name, including product name and trademark, that is associated to SimPad, SimDevices, SimGEKI, SimGETRO for promotional or other purposes. Please be kindly advised.

## Special Thanks

This project referenced these projects

- [ONGEEKi Firmware](https://github.com/GEEKiDoS/ongeeki-firmware) - GEEKiDoS
- [Arduino-Aime-Reader](https://github.com/Sucareto/Arduino-Aime-Reader) - Sucareto
- [Arduino-Aime-QReader](https://github.com/QHPaeek/Arduino-Aime-QReader) - QHPaeek