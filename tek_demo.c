/* tek_demo.c — Tektronix 4010 の最小サンプル
 *   ・画面消去 → 矩形 → 正弦波 → ラベル を描く
 * build: cc -O2 -o tek_demo tek_demo.c -lm
 * run  : xterm -t -hold -e ./tek_demo
 */
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define ESC "\x1b"
#define GS  "\x1d"   /* Graph mode へ */
#define US  "\x1f"   /* Alpha mode へ戻る */
#define FF  "\x0c"   /* Form Feed(ESC FF で画面消去)*/

/* 10-bit (x,y) を Tek の 4 バイトで送る。Lo-X で描画確定。 */
static void tek_xy(int x, int y) {
    putchar(0x20 | ((y >> 5) & 0x1f));   /* Hi-Y  001 YYYYY */
    putchar(0x60 | ( y       & 0x1f));   /* Lo-Y  011 YYYYY */
    putchar(0x20 | ((x >> 5) & 0x1f));   /* Hi-X  001 XXXXX */
    putchar(0x40 | ( x       & 0x1f));   /* Lo-X  010 XXXXX */
}

int main(void) {
    /* (1) 画面消去。xterm では ESC FF が同時に Tek ウィンドウへ
           スイッチするトリガにもなる。 */
    fputs(ESC FF, stdout);
    fflush(stdout);
    usleep(800000);                      /* ストレージチューブの消去待ち */

    /* (2) 矩形:GS の直後の最初の座標は move(描画なし)。
           以降の座標が直前点からの直線を描く。 */
    fputs(GS, stdout);
    tek_xy(150, 150);                    /* move */
    tek_xy(870, 150);                    /* draw → 下辺 */
    tek_xy(870, 600);                    /* draw → 右辺 */
    tek_xy(150, 600);                    /* draw → 上辺 */
    tek_xy(150, 150);                    /* draw → 左辺(閉じる) */

    /* (3) 矩形内に正弦波。新しい GS でカーソルをリセットして
           最初の点を move、続きを draw。 */
    fputs(GS, stdout);
    for (int x = 150; x <= 870; x += 4) {
        double t = (x - 150) / 720.0 * 4.0 * M_PI;
        int y = 375 + (int)(200.0 * sin(t));
        tek_xy(x, y);
    }

    /* (4) ラベル:GS で位置決め(最初の座標は move のみ)→ US で
           Alpha mode に戻ると、そこからテキストが書ける。 */
    fputs(GS, stdout);
    tek_xy(330, 680);
    fputs(US, stdout);
    fputs("HELLO TEKTRONIX 4010", stdout);

    fflush(stdout);
    return 0;
}
