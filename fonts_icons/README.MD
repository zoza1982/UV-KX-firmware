https://www.dafont.com/bitmap.php

## Convert TTF to bdf in FontForge and then to u8g2 font files using bdfconv_2_22.exe

## w=3, h=5
.\bdfconv_2_22.exe -v -f 1 -m "32-95" '.\Teeny-Tiny-Pixls-5.bdf' -o ../src/ui/font/font_5_tr.h  -n u8g2_font_5_tr


## w=7, h=8
.\bdfconv_2_22.exe -v -f 1 -m "32-125" '.\Uni0553-8.bdf' -o ../src/ui/font/font_8_tr.h  -n u8g2_font_8_tr
## w=8, h=8
.\bdfconv_2_22.exe -v -f 1 -m "32-125" '.\Uni0563-8.bdf' -o ../src/ui/font/font_8b_tr.h  -n u8g2_font_8b_tr

## w=6, h=9
.\bdfconv_2_22.exe -v -f 1 -m "48-57,43,45,46,58" '.\SmallBoldPixel-7-20.bdf' -o ../src/ui/font/font_10_tr.h  -n u8g2_font_10_tr


## w=7, h=10
.\bdfconv_2_22.exe -v -f 1 -m "48-57,43,45,46,58" '.\Blocktopia-16.bdf' -o ../src/ui/font/font_bn_tn.h  -n u8g2_font_bn_tn
