#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <locale.h>

const int WIDTH = 800;
const int HEIGHT = 600;

//координаты кнопки
const int btnX = WIDTH - 150;
const int btnY = 20;
const int btnW = 130;
const int btnH = 40;

//структура для треугольника
struct Triangle {
    int xStart;
    int xEnd;
    int height;
};

//фуункция рисования кнопки
void drawButton(Display* display, Window win, GC gc, XFontSet fontSet) {
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XDrawRectangle(display, win, gc, btnX, btnY, btnW, btnH);

    //текст
    const char* buttonText = "Сгенерировать";
    XmbDrawString(display, win, fontSet, gc, btnX + 10, btnY + 25, buttonText, strlen(buttonText));
}

//функция рисования гор
void drawMountains(Display* display, Window win, GC gc, XFontSet fontSet) {
    XClearWindow(display, win);

    std::vector<Triangle> mountains;
    int margin = 20;
    int attempts = 0;

    //случайная генерация размера горы
    while (mountains.size() < 3 && attempts < 100) {
        int baseWidth = 100 + rand() % 200;
        int height = 100 + rand() % 300;
        int x = rand() % (WIDTH - baseWidth - margin);

        bool intersects = false;
        for (auto &t : mountains) {
            if (!(x + baseWidth < t.xStart || x > t.xEnd)) {
                intersects = true;
                break;
            }
        }

        if (!intersects) {
            mountains.push_back({x, x + baseWidth, height});

            XPoint points[3];
            points[0].x = x;
            points[0].y = HEIGHT - 50;
            points[1].x = x + baseWidth / 2;
            points[1].y = HEIGHT - 50 - height;
            points[2].x = x + baseWidth;
            points[2].y = HEIGHT - 50;

            XDrawLine(display, win, gc, points[0].x, points[0].y, points[1].x, points[1].y);
            XDrawLine(display, win, gc, points[1].x, points[1].y, points[2].x, points[2].y);
        }
        attempts++;
    }

    drawButton(display, win, gc, fontSet);
}

int main() {
    setlocale(LC_ALL, ""); // Для русского языка
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

    //шрифт для русских символов
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
