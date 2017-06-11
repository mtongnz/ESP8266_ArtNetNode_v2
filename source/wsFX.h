/*
ESP8266_ArtNetNode v2.0.0
Copyright (c) 2016, Matthew Tong
https://github.com/mtongnz/ESP8266_ArtNetNode_v2

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.
If not, see http://www.gnu.org/licenses/
*/


#ifndef WS_FX
#define WS_FX

#include "ws2812Driver.h"

enum  pattern { STATIC, RAINBOW_CYCLE, THEATER_CHASE, TWINKLE };

class pixPatterns {
  public:
    pattern  ActivePattern;       // which pattern is running
    
    unsigned long Interval;       // milliseconds between updates
    unsigned long lastUpdate;     // last update of position
    
    uint32_t Colour1, Colour2;    // What colours are in use
    uint32_t Colour1Raw, Colour2Raw;    // Colours pre-intensity
    uint16_t TotalSteps;          // total number of steps in the pattern
    uint16_t Index;               // current step within the pattern
    uint8_t Speed;                 // speed of effect (0 = stop, -ve is reverse, +ve is forwards)
    uint8_t Size1, Size, Fade, Pos; // size, fading & position for static looks
    uint8_t Intensity;
    bool NewData;
    
    uint8_t Port;                 // port number.
    ws2812Driver* pixDriver;      // the pixel driver
    
    pixPatterns(uint8_t port, ws2812Driver* p);
    bool Update(void);
    void Increment(void);
    void setSpeed(uint8_t s);
    void setIntensity(uint8_t i);
    void setColour1(uint32_t c);
    void setColour2(uint32_t c);
    void setFX(uint8_t fx);
    void Static(void);
    void StaticUpdate(void);
    void RainbowCycle(void);
    void RainbowCycleUpdate(void);
    void TheaterChase(void);
    void TheaterChaseUpdate(void);
    void Twinkle(void);
    void TwinkleUpdate(void);
    uint32_t DimColour(uint32_t colour);
    uint32_t Colour(uint8_t r, uint8_t g, uint8_t b);
    uint8_t Red(uint32_t colour);
    uint8_t Green(uint32_t colour);
    uint8_t Blue(uint32_t colour);
    uint32_t Wheel(byte WheelPos);
};

#endif
