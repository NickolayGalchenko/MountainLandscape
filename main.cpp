#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>
#include <locale.h>

const int WIDTH = 800;
const int HEIGHT = 600;

// координаты кнопки
const int btnX = WIDTH - 150;
const int btnY = 20;
const int btnW = 130;
const int btnH = 40;

// структура для треугольника
struct Triangle {
    int xStart;
    int xEnd;
    int height;
};

// функция рисования кнопки
void drawButton(Display* display, Window win, GC gc, XFontSet fontSet) {
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XDrawRectangle(display, win, gc, btnX, btnY, btnW, btnH);

    const char* buttonText = "Сгенерировать";
    XmbDrawString(display, win, fontSet, gc, btnX + 10, btnY + 25, buttonText, strlen(buttonText));
}

// функция рисования гор
void drawMountains(Display* display, Window win, GC gc, XFontSet fontSet) {
    XClearWindow(display, win);

    std::vector<Triangle> mountains;

    // генерируем ровно 3 случайные горы
    for (int i = 0; i < 3; ++i) {
        int baseWidth = 100 + rand() % 200;
        int height = 100 + rand() % 300;
        int x = rand() % (WIDTH - baseWidth - 20);
        mountains.push_back({x, x + baseWidth, height});
    }

    // рисуем горы по порядку
    for (auto &t : mountains) {
        XPoint mountain[3];
        mountain[0].x = t.xStart;
        mountain[0].y = HEIGHT - 50;
        mountain[1].x = (t.xStart + t.xEnd) / 2;
        mountain[1].y = HEIGHT - 50 - t.height;
        mountain[2].x = t.xEnd;
        mountain[2].y = HEIGHT - 50;

        // серый цвет для горы
        XSetForeground(display, gc, 0x888888);
        XFillPolygon(display, win, gc, mountain, 3, Convex, CoordModeOrigin);

        // белая снежная шапка
        int peakX = (t.xStart + t.xEnd) / 2;
        int peakY = HEIGHT - 50 - t.height;
        int snowWidth = t.xEnd - t.xStart;
        int snowHeight = t.height / 4;

        XPoint snow[3];
        snow[0].x = peakX - snowWidth / 6;
        snow[0].y = peakY + snowHeight;
        snow[1].x = peakX;
        snow[1].y = peakY;
        snow[2].x = peakX + snowWidth / 6;
        snow[2].y = peakY + snowHeight;

        XSetForeground(display, gc, 0xFFFFFF);
        XFillPolygon(display, win, gc, snow, 3, Convex, CoordModeOrigin);

        // контур горы
        XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
        XDrawLine(display, win, gc, mountain[0].x, mountain[0].y, mountain[1].x, mountain[1].y);
        XDrawLine(display, win, gc, mountain[1].x, mountain[1].y, mountain[2].x, mountain[2].y);
    }

    drawButton(display, win, gc, fontSet);
}

int main() {
    setlocale(LC_ALL, "");
    srand(time(nullptr));

    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Cannot open display\n";
        return 1;
    }

    int screen = DefaultScreen(display);
    Window win = XCreateSimpleWindow(display, RootWindow(display, screen),
                                     100, 100, WIDTH, HEIGHT, 1,
                                     BlackPixel(display, screen),
                                     WhitePixel(display, screen));

    XSelectInput(display, win, ExposureMask | ButtonPressMask);
    XMapWindow(display, win);

    GC gc = XCreateGC(display, win, 0, nullptr);
    XSetForeground(display, gc, BlackPixel(display, screen));

    char** missing;
    int missing_count;
    XFontSet fontSet = XCreateFontSet(display, "-*-fixed-*-*-*-*-*-*-*-*-*-*-*-*", &missing, &missing_count, nullptr);
    if (!fontSet) {
        std::cerr << "Cannot create font set\n";
        return 1;
    }

    drawButton(display, win, gc, fontSet);

    while (true) {
        XEvent e;
        XNextEvent(display, &e);

        if (e.type == Expose) {
            drawButton(display, win, gc, fontSet);
        } else if (e.type == ButtonPress) {
            int mx = e.xbutton.x;
            int my = e.xbutton.y;

            if (mx >= btnX && mx <= btnX + btnW && my >= btnY && my <= btnY + btnH) {
                drawMountains(display, win, gc, fontSet);
            }
        }
    }

    XFreeGC(display, gc);
    XFreeFontSet(display, fontSet);
    XCloseDisplay(display);
    return 0;
}
