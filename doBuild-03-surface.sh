#!/bin/bash
# WSLg の X server には Tek 用ビットマップフォント (9x15/8x13/6x10) が無く
# `Warning: Cannot convert string "..." to type FontStruct` が出るので、
# 代わりに `fixed` を使うよう xrm で上書き。
TEKFONTS=(
  -xrm 'XTerm*tek4014*fontLarge: fixed'
  -xrm 'XTerm*tek4014*font2: fixed'
  -xrm 'XTerm*tek4014*font3: fixed'
  -xrm 'XTerm*tek4014*fontSmall: fixed'
)

cc -O2 -o tek_surface tek_surface.c -lm
xterm -t -hold -geometry 1024x780 "${TEKFONTS[@]}" -e ./tek_surface
