#pragma once

namespace Theme {

struct Color {
    int r;
    int g;
    int b;
};

enum class Mode {
    Day = 0,
    Night = 1
};

Mode getMode();
void setMode(Mode newMode);
bool isNight();

Color background();
Color hudText();
Color trackLine();
Color nearestLine();
Color trackShadow(int grey);
Color progressFrame();
Color progressFill();
Color forkGrey();
Color lineDarkBlue();
Color lineDarkRed();
Color lineGrey();
Color lineDeepRed();

}
