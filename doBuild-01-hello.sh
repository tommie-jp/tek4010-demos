#!/bin/bash
# WSLg の X server には Tek 用ビットマップフォント (9x15/8x13/6x10) が無く
# `Warning: Cannot convert string "..." to type FontStruct` が出るので、
# 代わりに `fixed` (どの X server にも必ずある) を使うよう xrm で上書き。
TEKFONTS=(
  -xrm 'XTerm*tek4014*fontLarge: fixed'
  -xrm 'XTerm*tek4014*font2: fixed'
  -xrm 'XTerm*tek4014*font3: fixed'
  -xrm 'XTerm*tek4014*fontSmall: fixed'
)

xterm -t -hold "${TEKFONTS[@]}" -e bash -c $'printf \'\\x1b\\x0c\\x1d\\x24\\x60\\x24\\x40\\x24\\x60\\x3b\\x40\\x34\\x60\\x3b\\x40\\x34\\x60\\x24\\x40\\x24\\x60\\x24\\x40\\x1d\\x28\\x60\\x28\\x40\\x1fHELLO, WORLD!\'; sleep 3'
