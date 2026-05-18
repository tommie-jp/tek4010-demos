/* tek_surface.c — Tektronix 4010 で 3D ワイヤーフレーム曲面を描く
 *   sombrero function:  z = sin(r) / r,  r = sqrt(x²+y²)
 *
 * build: cc -O2 -o tek_surface tek_surface.c -lm
 * run  : xterm -t -hold -geometry 1024x780 -e ./tek_surface
 */
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define ESC "\x1b"
#define GS  "\x1d"
#define US  "\x1f"
#define FF  "\x0c"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Tek 4010 座標送信(10-bit、4バイト)*/
static void tek_xy(int x, int y) {
    if (x < 0) x = 0; if (x > 1023) x = 1023;
    if (y < 0) y = 0; if (y > 779)  y = 779;
    putchar(0x20 | ((y >> 5) & 0x1f));
    putchar(0x60 | ( y       & 0x1f));
    putchar(0x20 | ((x >> 5) & 0x1f));
    putchar(0x40 | ( x       & 0x1f));
}

/* sombrero 関数 */
static double f(double x, double y) {
    double r = sqrt(x*x + y*y);
    return (r < 1e-6) ? 1.0 : sin(r) / r;
}

/* (wx,wy,wz) world → (sx,sy) Tek screen に正射影
 *   world: x=右、y=奥、z=上
 *   azimuth = Z軸まわり回転(視点の方位)
 *   elev    = X軸まわり回転(見下ろし角度)
 */
static void project(double wx, double wy, double wz, int *sx, int *sy) {
    const double azimuth = 35.0 * M_PI / 180.0;
    const double elev    = 20.0 * M_PI / 180.0;
    const double scale   = 38.0;
    const double cx = 512, cy = 360;

    /* azimuth: Z軸まわり */
    double ca = cos(azimuth), sa = sin(azimuth);
    double x1 =  ca * wx + sa * wy;
    double y1 = -sa * wx + ca * wy;

    /* elevation: X軸まわりで傾けて画面上方向に投影 */
    double ce = cos(elev), se = sin(elev);
    double sxw = x1;
    double syw = wz * ce + y1 * se;

    *sx = (int)(cx + sxw * scale);
    *sy = (int)(cy + syw * scale);
}

int main(void) {
    const int N = 45;                 /* メッシュ解像度 */
    const double range  = 8.5;        /* x,y in [-range, range] */
    const double zscale = 5.0;        /* 高さ強調倍率 */

    /* (1) 画面消去 */
    fputs(ESC FF, stdout);
    fflush(stdout);
    usleep(800000);

    /* (2) タイトル */
    int sx, sy;
    fputs(GS, stdout);
    tek_xy(260, 730);
    fputs(US, stdout);
    fputs("Z = SIN(R) / R     R = SQRT(X**2 + Y**2)", stdout);

    /* (3) ベース矩形(z=0 平面のバウンディング)*/
    fputs(GS, stdout);
    project(-range, -range, 0, &sx, &sy); tek_xy(sx, sy);
    project( range, -range, 0, &sx, &sy); tek_xy(sx, sy);
    project( range,  range, 0, &sx, &sy); tek_xy(sx, sy);
    project(-range,  range, 0, &sx, &sy); tek_xy(sx, sy);
    project(-range, -range, 0, &sx, &sy); tek_xy(sx, sy);

    /* (4) X 方向の格子線(y固定で x を走らせる)*/
    for (int j = 0; j < N; j++) {
        double y = -range + 2.0 * range * j / (N - 1);
        fputs(GS, stdout);
        for (int i = 0; i < N; i++) {
            double x = -range + 2.0 * range * i / (N - 1);
            project(x, y, zscale * f(x, y), &sx, &sy);
            tek_xy(sx, sy);
        }
    }

    /* (5) Y 方向の格子線(x固定で y を走らせる)*/
    for (int i = 0; i < N; i++) {
        double x = -range + 2.0 * range * i / (N - 1);
        fputs(GS, stdout);
        for (int j = 0; j < N; j++) {
            double y = -range + 2.0 * range * j / (N - 1);
            project(x, y, zscale * f(x, y), &sx, &sy);
            tek_xy(sx, sy);
        }
    }

    /* (6) 軸 + ラベル */
    fputs(GS, stdout);
    project(-range, -range, 0,             &sx, &sy); tek_xy(sx, sy);
    project(-range, -range, zscale * 1.15, &sx, &sy); tek_xy(sx, sy);
    fputs(US, stdout); fputs(" Z", stdout);

    fputs(GS, stdout);
    project( range, -range, 0, &sx, &sy); tek_xy(sx, sy);
    fputs(US, stdout); fputs(" X", stdout);

    fputs(GS, stdout);
    project(-range,  range, 0, &sx, &sy); tek_xy(sx, sy);
    fputs(US, stdout); fputs(" Y", stdout);

    fflush(stdout);
    return 0;
}
