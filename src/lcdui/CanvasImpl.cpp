#include "CanvasImpl.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdexcept>
#include <iostream>

#include "Canvas.h"

#ifdef VITA_PORT
#include "../Micro.h"
#endif

CanvasImpl::CanvasImpl(Canvas* canvas)
{
    this->canvas = canvas;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        throw std::runtime_error(SDL_GetError());
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        throw std::runtime_error(IMG_GetError());
    }

    if (TTF_Init() == -1) {
        throw std::runtime_error(TTF_GetError());
    }

#ifdef VITA_PORT
    // SDL2 does not generate any joystick events unless a joystick is opened,
    // and on the Vita every button/stick input arrives as joystick input.
    SDL_JoystickEventState(SDL_ENABLE);
    if (SDL_NumJoysticks() > 0) {
        joystick = SDL_JoystickOpen(0);
    }
#endif

    window = SDL_CreateWindow(
        0,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_SHOWN);

    if (!window) {
        throw std::runtime_error(SDL_GetError());
    }

    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

CanvasImpl::~CanvasImpl()
{
#ifdef VITA_PORT
    if (joystick) {
        SDL_JoystickClose(joystick);
        joystick = nullptr;
    }
#endif
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}

void CanvasImpl::repaint()
{
    SDL_RenderPresent(renderer);
}

int CanvasImpl::getWidth()
{
    return width;
}

int CanvasImpl::getHeight()
{
    return height;
}

SDL_Renderer* CanvasImpl::getRenderer()
{
    return renderer;
}

void CanvasImpl::processEvents()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        switch (e.type) {
        case SDL_QUIT:
            exit(0); // IMPROVE This is a super dumb way to finish the game, but it works
            break;
        case SDL_KEYDOWN: {
            int keyCode = convertKeyCharToKeyCode(e.key.keysym.sym);
            // std::cout << "Key pressed: " << keyCode << std::endl;
            if (keyCode != 0) {
                canvas->publicKeyPressed(keyCode);
            }
        } break;
        case SDL_KEYUP: {
            int sdlCode = e.key.keysym.sym;
            int keyCode = convertKeyCharToKeyCode(sdlCode);
            // std::cout << "Key released: " << keyCode << std::endl;
            if (keyCode != 0) {
                canvas->publicKeyReleased(keyCode);
            } else {
                if (sdlCode == SDLK_ESCAPE) {
                    // std::cout << "ESC released" << std::endl;
                    canvas->pressedEsc();
                }
            }
        } break;
#ifdef VITA_PORT
        case SDL_JOYBUTTONDOWN:
            handleVitaJoyButton(e.jbutton.button, true);
            break;
        case SDL_JOYBUTTONUP:
            handleVitaJoyButton(e.jbutton.button, false);
            break;
        case SDL_JOYAXISMOTION:
            handleVitaJoyAxis(e.jaxis.axis, e.jaxis.value);
            break;
#endif
        default:
            break;
        }
    }
}

#ifdef VITA_PORT
void CanvasImpl::sendVitaKey(int keyCode, bool pressed)
{
    if (keyCode == 0) {
        return;
    }

    if (pressed) {
        canvas->publicKeyPressed(keyCode);
    } else {
        canvas->publicKeyReleased(keyCode);
    }
}

void CanvasImpl::handleVitaJoyButton(unsigned button, bool pressed)
{
    switch (button) {
    case 0: // Triangle: FIRE (select) in menus, reverse in game
        if (Micro::isInGameMenu) {
            sendVitaKey(Canvas::Keys::FIRE, pressed);
        } else {
            sendVitaKey(Canvas::Keys::REVERSE, pressed);
        }
        break;
    case 2: // Cross: FIRE (select) in menus, gas in game
        if (Micro::isInGameMenu) {
            sendVitaKey(Canvas::Keys::FIRE, pressed);
        } else {
            sendVitaKey(Canvas::Keys::UP, pressed);
        }
        break;
    case 3: // Square: brake in game, down in menus
    case 6: // D-pad down
        sendVitaKey(Canvas::Keys::DOWN, pressed);
        break;
    case 8: // D-pad up: gas / up in menus
        sendVitaKey(Canvas::Keys::UP, pressed);
        break;
    case 4: // L1
    case 12: // L2: up in menus, reverse in game
        if (Micro::isInGameMenu) {
            sendVitaKey(Canvas::Keys::UP, pressed);
        } else {
            sendVitaKey(Canvas::Keys::REVERSE, pressed);
        }
        break;
    case 5: // R1
    case 13: // R2: down in menus, gas in game
        if (Micro::isInGameMenu) {
            sendVitaKey(Canvas::Keys::DOWN, pressed);
        } else {
            sendVitaKey(Canvas::Keys::UP, pressed);
        }
        break;
    case 7: // D-pad left
        sendVitaKey(Canvas::Keys::LEFT, pressed);
        break;
    case 9: // D-pad right
        sendVitaKey(Canvas::Keys::RIGHT, pressed);
        break;
    case 1: // Circle: back
    case 11: // Start: open menu / back
        if (!pressed) {
            canvas->pressedEsc();
        }
        break;
    default:
        break;
    }
}

void CanvasImpl::handleVitaJoyAxis(unsigned axis, int value)
{
    // Only the left stick is used (axis 0 - X, axis 1 - Y)
    if (axis > 1) {
        return;
    }

    const int deadzone = 12000;
    bool neg = value < -deadzone;
    bool pos = value > deadzone;

    if (axis == 0) {
        if (neg != stickLeft) {
            stickLeft = neg;
            sendVitaKey(Canvas::Keys::LEFT, neg);
        }
        if (pos != stickRight) {
            stickRight = pos;
            sendVitaKey(Canvas::Keys::RIGHT, pos);
        }
    } else {
        if (neg != stickUp) {
            stickUp = neg;
            sendVitaKey(Canvas::Keys::UP, neg);
        }
        if (pos != stickDown) {
            stickDown = pos;
            sendVitaKey(Canvas::Keys::DOWN, pos);
        }
    }
}
#endif

int CanvasImpl::convertKeyCharToKeyCode(SDL_Keycode keyCode)
{
    switch (keyCode) {
    case SDLK_RETURN:
        return Canvas::Keys::FIRE;
    case SDLK_LEFT:
        return Canvas::Keys::LEFT;
    case SDLK_RIGHT:
        return Canvas::Keys::RIGHT;
    case SDLK_UP:
        return Canvas::Keys::UP;
    case SDLK_DOWN:
        return Canvas::Keys::DOWN;
    default:
        std::cout << "unknown keyEvent: " << keyCode << std::endl;
        return 0;
    }
}

void CanvasImpl::setWindowTitle(const std::string& title)
{
    SDL_SetWindowTitle(window, title.c_str());
}