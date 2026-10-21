#pragma once

#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

class Canvas;

class CanvasImpl {
private:
    Canvas* canvas;

    SDL_Window* window;
    SDL_Renderer* renderer;

#ifdef VITA_PORT
    // Native physical screen resolution.
    const int width = 960;
    const int height = 544;
#else
    const int width = 640;
    const int height = 480;
#endif

    static int convertKeyCharToKeyCode(SDL_Keycode keyCode);

#ifdef VITA_PORT
    void handleVitaJoyButton(unsigned button, bool pressed);
    void handleVitaJoyAxis(unsigned axis, int value);
    void sendVitaKey(int keyCode, bool pressed);
    SDL_Joystick* joystick = nullptr;
    bool stickLeft = false;
    bool stickRight = false;
    bool stickUp = false;
    bool stickDown = false;
#endif

public:
    CanvasImpl(Canvas* canvas);
    ~CanvasImpl();

    void repaint();
    int getWidth();
    int getHeight();

    SDL_Renderer* getRenderer();
    void processEvents();
    void setWindowTitle(const std::string& title);
};