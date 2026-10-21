#pragma once

#include <memory>
#include <string>

class Graphics;
class Font;

class IGameMenuElement {
public:
    virtual void setText(std::string text) = 0;
    virtual void render(Graphics* graphics, int y, int x) = 0;
    virtual bool isNotTextRender() = 0;
    virtual void menuElemMethod(int var1) = 0;
    virtual int measureWidth(std::shared_ptr<Font> font) { (void)font; return 0; }
};
