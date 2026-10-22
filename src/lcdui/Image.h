#pragma once

#include <string>
#include <SDL2/SDL.h>

class Image {
private:
    SDL_Surface* surface;
    SDL_Texture* texture = nullptr;
    SDL_Renderer* textureRenderer = nullptr;

public:
    Image(const std::string& embeddedPath);
    Image(int width, int height);
    ~Image();

    int getWidth() const;
    int getHeight() const;
    SDL_Surface* getSurface() const;
    SDL_Texture* getTexture(SDL_Renderer* renderer);
    void invertLuminance();
};
