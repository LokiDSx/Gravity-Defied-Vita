#include "Image.h"

#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmrc/cmrc.hpp>

CMRC_DECLARE(assets);

Image::Image(int width, int height)
{
    SDL_Surface* surf = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (!surf) {
        throw std::runtime_error(SDL_GetError());
    }

    this->surface = surf;
}

Image::Image(const std::string& embeddedPath)
{
    cmrc::embedded_filesystem embeddedFs = cmrc::assets::get_filesystem();
    cmrc::file fileData = embeddedFs.open(embeddedPath);

    SDL_RWops* raw = SDL_RWFromConstMem(fileData.begin(), fileData.size());
    if (!raw) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_Surface* surf = IMG_Load_RW(raw, SDL_TRUE);
    if (!surf) {
        throw std::runtime_error(IMG_GetError());
    }

    SDL_Surface* surf_conv = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surf);

    if (!surf_conv) {
        throw std::runtime_error(SDL_GetError());
    }

    this->surface = surf_conv;
}

Image::~Image()
{
    if (this->texture) {
        SDL_DestroyTexture(this->texture);
    }
    SDL_FreeSurface(this->surface);
}

int Image::getWidth() const
{
    return this->surface->w;
}

int Image::getHeight() const
{
    return this->surface->h;
}

SDL_Surface* Image::getSurface() const
{
    return this->surface;
}

SDL_Texture* Image::getTexture(SDL_Renderer* renderer)
{
    if (this->texture == nullptr || this->textureRenderer != renderer) {
        if (this->texture) {
            SDL_DestroyTexture(this->texture);
        }
        this->texture = SDL_CreateTextureFromSurface(renderer, this->surface);
        this->textureRenderer = renderer;
    }

    return this->texture;
}

void Image::invertLuminance()
{
    if (!this->surface) {
        return;
    }

    if (SDL_MUSTLOCK(this->surface)) {
        SDL_LockSurface(this->surface);
    }

    for (int y = 0; y < this->surface->h; ++y) {
        Uint8* row = static_cast<Uint8*>(this->surface->pixels) + y * this->surface->pitch;
        for (int x = 0; x < this->surface->w; ++x) {
            Uint8* px = row + x * 4;
            px[0] = static_cast<Uint8>(255 - px[0]);
            px[1] = static_cast<Uint8>(255 - px[1]);
            px[2] = static_cast<Uint8>(255 - px[2]);
        }
    }

    if (SDL_MUSTLOCK(this->surface)) {
        SDL_UnlockSurface(this->surface);
    }

    if (this->texture) {
        SDL_DestroyTexture(this->texture);
        this->texture = nullptr;
    }
}
