#!/bin/bash

firmwareFile=$(echo $1 | sed 's/^\([a-zA-Z]\)\(.*\)/\2/g; s/://g' | sed 's/\\/\//g')

echo "--------------------------------------------------"
echo "firmware: ${firmwareFile}"
echo "--------------------------------------------------"

./tools/wch_openocd/bin/openocd -f ./tools/wch_openocd/bin/wch-riscv.cfg \
    -c init -c halt \
    -c "flash erase_sector wch_riscv 0 last" \
    -c "program ${firmwareFile}" \
    -c "verify_image ${firmwareFile}" \
    -c wlink_reset_resume -c exit
