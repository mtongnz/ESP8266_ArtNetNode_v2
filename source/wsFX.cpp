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


#include "wsFX.h"

pixPatterns::pixPatterns(uint8_t port, ws2812Driver* p) {
  pixDriver = p;
  Port = port;
  NewData = 0;
  lastUpdate = 0;
  Intensity = 0;
  Speed = 0;
  TotalSteps = 100;
}
    
// Update the pattern
bool pixPatterns::Update(void) {
  if((millis() - lastUpdate) > Interval) { // time to update
    lastUpdate = millis();
    
    switch(ActivePattern) {
      case RAINBOW_CYCLE:
        RainbowCycleUpdate();
        break;
      case THEATER_CHASE:
        TheaterChaseUpdate();
        break;
      case TWINKLE:
        TwinkleUpdate();
        break;
      
      case STATIC:
      default:
        StaticUpdate();
        break;
    }
    return 1;
  }
  return 0;
}
  
// Increment the Index and reset at the end
void pixPatterns::Increment(void) {
  if (Speed < 20 || Speed > 235)
    return;
    
  else if (Speed > 131) {
    Index++;
    
    if (Index >= TotalSteps)
      Index = 0;
  
  } else if (Speed < 123) {
    if (Index == 0 || Index > TotalSteps)
      Index = TotalSteps - 1;
    else
      Index--;
  }
}

void pixPatterns::setSpeed(uint8_t s) {
  // Index reset mode
  if (s < 20) {
    if (Speed != s)
      Index = 0;
    Speed = s;
    Interval = 1;

  // Indexed non-reset mode
  } else if (s > 235) {
    Interval = 1;
    Speed = s;

  // Chase mode
  } else {
    Speed = s;
    if (Speed > 127)
      Interval = map(Speed, 131, 235, 1, 60);
    else
      Interval = map(Speed, 20, 123, 60, 1);
  }
}

void pixPatterns::setIntensity(uint8_t i) {
  Intensity = i;
  setColour1(Colour1Raw);
  setColour2(Colour2Raw);
}

void pixPatterns::setColour1(uint32_t c) {
  Colour1Raw = c;
  Colour1 = Colour(map(Red(Colour1Raw), 0, 255, 0, Intensity), map(Green(Colour1Raw), 0, 255, 0, Intensity), map(Blue(Colour1Raw), 0, 255, 0, Intensity));
}

void pixPatterns::setColour2(uint32_t c) {
  Colour2Raw = c;
  Colour2 = Colour(map(Red(Colour2Raw), 0, 255, 0, Intensity), map(Green(Colour2Raw), 0, 255, 0, Intensity), map(Blue(Colour2Raw), 0, 255, 0, Intensity));
}

void pixPatterns::setFX(uint8_t fx) {
  if (fx < 50)
    Static();
  else if (fx < 75)
    RainbowCycle();
  else if (fx < 100)
    TheaterChase();
  else if (fx < 125)
    Twinkle();
}

// Initialize static looks
void pixPatterns::Static(void) {
  if (ActivePattern == STATIC)
    return;
  
  ActivePattern = STATIC;
  Index = 0;
}

// Update the static look
void pixPatterns::StaticUpdate(void) {
  TotalSteps = pixDriver->numPixels(Port);

  // Calculate the values to use mapped to the number of pixels we have
  uint16_t mSize = map(Size, 0, 255, 2, TotalSteps);           // Overall size
  uint16_t mSize1 = map(Size1, 0, 255, 0, mSize);                               // Colour1 size
//  uint16_t mFade = map(Fade, 0, 255, 0, (mSize/2));                             // Colour fade size

  // Calculate the position offset - the shapes are centered using Pos
  uint16_t midPoint = map(Pos, 0, 255, 0, TotalSteps);
  uint16_t mPos = mSize - (midPoint - (mSize/2) - (uint16_t)((midPoint - (mSize/2)) / mSize) * mSize) + Index;
/* 
  // Calculate the colour components
  uint8_t r1 = Red(Colour1);
  uint8_t g1 = Green(Colour1);
  uint8_t b1 = Blue(Colour1);
  uint8_t r2 = Red(Colour2);
  uint8_t g2 = Green(Colour2);
  uint8_t b2 = Blue(Colour2);
  int16_t r3, g3, b3;

  // Calculate fade values
  if (mFade) {
    r3 = (r2 - r1) / mFade;
    g3 = (g2 - g1) / mFade;
    b3 = (b2 - b1) / mFade;
  }
*/
  
  for(uint16_t p = 0; p < TotalSteps; p++) {
    uint16_t i = (p + mPos) % mSize;
    uint32_t c;
/*
    // Left faded area
    if (mFade && i < mFade) {
      uint8_t r = (r3 * i) + r1;
      uint8_t g = (g3 * i) + g1;
      uint8_t b = (b3 * i) + b1;

      c = Colour(r, g, b);

    // Middle faded area
    } else if (mFade && i > (mSize1 - mFade) && i < (mSize1 + mFade)) {
      i -= (mSize1 - mFade);
      uint8_t r = (r3 * i) + r1;
      uint8_t g = (g3 * i) + g1;
      uint8_t b = (b3 * i) + b1;
      
      c = Colour(r, g, b);

    // Middle faded area
    } else if (mFade && i < (mSize1 + mFade)) {
      i = (mSize1 + mFade) - i;
      uint8_t r = (r3 * i) + r2;
      uint8_t g = (g3 * i) + g2;
      uint8_t b = (b3 * i) + b2;
      
      c = Colour(r, g, b);

    // Right faded area
    } else if (mFade && i > (mSize - mFade)) {
      i -= (mSize - mFade);
      uint8_t r = (r3 * i) + r2;
      uint8_t g = (g3 * i) + g2;
      uint8_t b = (b3 * i) + b2;
      
      c = Colour(r, g, b);
      
    // Out of faded area
    } else
*/ 
    if (i < mSize1)
      c = Colour1;
    else
      c = Colour2;
    
    pixDriver->setPixel(Port, p, c);
  }
  Increment();
}

// Initialize for a RainbowCycle
void pixPatterns::RainbowCycle(void) {
  if (ActivePattern == RAINBOW_CYCLE)
    return;
  
  ActivePattern = RAINBOW_CYCLE;
  Index = 0;
}

// Update the Rainbow Cycle Pattern
void pixPatterns::RainbowCycleUpdate(void) {
  TotalSteps = 255;
  
  uint16_t mSize = map(Size, 0, 255, 2, pixDriver->numPixels(Port));
  
  for(uint16_t p = 0; p < pixDriver->numPixels(Port);) {
    for (uint16_t i = 0; i < mSize && p < pixDriver->numPixels(Port); i++, p++) {
      uint32_t c = Wheel(((i * 256 / mSize) + Index + Pos) & 255);
      uint8_t r = map(Red(c), 0, 255, 0, Intensity);
      uint8_t g = map(Green(c), 0, 255, 0, Intensity);
      uint8_t b = map(Blue(c), 0, 255, 0, Intensity);
      
      pixDriver->setPixel(Port, p, Colour(r, g, b));
    }
  }
  Increment();
}

// Initialize for a Theater Chase
void pixPatterns::TheaterChase(void) {
  if (ActivePattern == THEATER_CHASE)
    return;
  
  ActivePattern = THEATER_CHASE;
  Index = 0;
}

// Update the Theater Chase Pattern
void pixPatterns::TheaterChaseUpdate(void) {
  TotalSteps = pixDriver->numPixels(Port);
  
  uint8_t mSize = map(Size, 0, 255, 3, 50);
  uint8_t a = (Index / map(mSize, 3, 50, 8, 2)) + map(Pos, 0, 255, mSize, 0);
  
  for(int i = 0; i < pixDriver->numPixels(Port); i++) {
    if ((i + a) % mSize == 0)
      pixDriver->setPixel(Port, i, Colour1);
    else
      pixDriver->setPixel(Port, i, Colour2);
  }
  Increment();
}

// Initialize for Twinkle
void pixPatterns::Twinkle(void) {
  if (ActivePattern == TWINKLE)
    return;
  
  ActivePattern = TWINKLE;
  Index = 0;

  randomSeed(analogRead(0));
}

// Update the Twinkle Pattern
void pixPatterns::TwinkleUpdate(void) {
  TotalSteps = 3;
  
  // Clear strip
  if (Index % 3 == 0 || Speed < 20 || Speed > 235) {
    for (uint16_t i = 0; i < pixDriver->numPixels(Port); i++)
      pixDriver->setPixel(Port, i, Colour1);
  }

  // Make twinkles
  if (Index % 3 == 0 && Speed > 20 && Speed < 235) {
    uint16_t numTwinks = map(Size, 0, 255, 1, (pixDriver->numPixels(Port) / 10));
    for (uint8_t n = 0; n < numTwinks; n++)
      pixDriver->setPixel(Port, random(0, pixDriver->numPixels(Port)), Colour2);
  }
  
  Increment();
}

// Calculate 50% dimmed version of a colour (used by ScannerUpdate)
uint32_t pixPatterns::DimColour(uint32_t colour) {
  return ((colour & 0xFEFEFE) >> 1);
}

// Returns 32-bit colour from components
uint32_t pixPatterns::Colour(uint8_t r, uint8_t g, uint8_t b) {
  return ((r << 16) | (g << 8) | b);
}

// Returns the Red component of a 32-bit colour
uint8_t pixPatterns::Red(uint32_t colour) {
  return (colour >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit colour
uint8_t pixPatterns::Green(uint32_t colour) {
  return (colour >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit colour
uint8_t pixPatterns::Blue(uint32_t colour) {
  return colour & 0xFF;
}

// Input a value 0 to 255 to get a colour value.
// The colours are a transition r - g - b - back to r.
uint32_t pixPatterns::Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85)
    return Colour(255 - WheelPos * 3, 0, WheelPos * 3);
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return Colour(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return Colour(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

