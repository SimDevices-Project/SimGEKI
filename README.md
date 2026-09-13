# Firmware for SimGETRO (CH32V203 based)

[简体中文](./README_zh-hans.md)

The repository for firmware of SimGETRO/SimGEKI designed based on the ch32v203 chip.

## License

Unless a file or directory states otherwise, the contributor-owned portions of
this repository are source-available under the
[PolyForm Noncommercial License 1.0.0](./LICENSE). It permits use, modification,
and distribution for noncommercial purposes. Commercial use is not licensed by
these terms and requires a separate written license from the project owner;
please contact the owner through the
[SimGEKI repository](https://github.com/iamapig120/SimGEKI).

PolyForm Noncommercial is not an OSI-approved open-source license. Third-party
components, including files bundled under `sdk/` and `tools/`, remain governed
by their own license notices.

## Compiling

Currently, you need to install Risc-GCC (WCH) in order to compile the firmware correctly.
Also, you might need VS Code & Embedded IDE plugin as your IDE progrom.
Use WCHISPtool to burn firmware to your device.

## Files

You could refer to the following table to find what you're looking for.

| Folder | Content                                 |
| ------ | --------------------------------------- |
| src    | Firmware source code                    |
| sdk    | CH32V SDK                               |
| ref    | Reference, such as src files for JLCEDA |
| tools  | Windows tool chains                     |

## Build Yourself

[SimDevices Wiki Page](https://sim.bysb.net/#/simgetro/diy/core_and_firmware/)
Check this page to get your own SimGEKI, Sch files, Pcb files even Cad files all included!

## Trademarks

The software license does not grant rights to the SimPad, SimDevices, SimGEKI,
or SimGETRO names or trademarks. Do not use them to market a product or imply
sponsorship, endorsement, or affiliation without prior permission.

## Special Thanks

This project referenced these projects

- [ONGEEKi Firmware](https://github.com/GEEKiDoS/ongeeki-firmware) - GEEKiDoS
- [Arduino-Aime-Reader](https://github.com/Sucareto/Arduino-Aime-Reader) - Sucareto
- [Arduino-Aime-QReader](https://github.com/QHPaeek/Arduino-Aime-QReader) - QHPaeek
