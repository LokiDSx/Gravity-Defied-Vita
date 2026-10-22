#include "Theme.h"

namespace Theme {

static Mode currentMode = Mode::Day;

Mode getMode()
{
    return currentMode;
}

void setMode(Mode newMode)
{
    currentMode = newMode;
}

bool isNight()
{
    return currentMode == Mode::Night;
}

Color background()
{
    return isNight() ? Color { 24, 24, 32 } : Color { 255, 255, 255 };
}

Color hudText()
{
    return isNight() ? Color { 255, 255, 255 } : Color { 0, 0, 0 };
}

Color trackLine()
{
    return isNight() ? Color { 70, 130, 255 } : Color { 0, 170, 0 };
}

Color nearestLine()
{
    return isNight() ? Color { 120, 190, 255 } : Color { 0, 255, 0 };
}

Color trackShadow(int grey)
{
    return isNight() ? Color { grey / 14, grey / 14, grey / 12 } : Color { grey, grey, grey };
}

Color progressFrame()
{
    return isNight() ? Color { 255, 255, 255 } : Color { 0, 0, 0 };
}

Color progressFill()
{
    return isNight() ? Color { 70, 130, 255 } : Color { 255, 255, 255 };
}

Color forkGrey()
{
    return isNight() ? Color { 200, 200, 200 } : Color { 128, 128, 128 };
}

Color lineDarkBlue()
{
    return isNight() ? Color { 100, 140, 255 } : Color { 0, 0, 128 };
}

Color lineDarkRed()
{
    return isNight() ? Color { 255, 90, 90 } : Color { 156, 0, 0 };
}

Color lineGrey()
{
    return isNight() ? Color { 170, 170, 170 } : Color { 50, 50, 50 };
}

Color lineDeepRed()
{
    return isNight() ? Color { 255, 90, 90 } : Color { 170, 0, 0 };
}

}
