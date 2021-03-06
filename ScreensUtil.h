#ifndef __SCREENSUTIL_H
#define __SCREENSUTIL_H


constexpr int16_t MARGIN = 10;
constexpr int DEFAULT_TEXT_SCALE=2;
constexpr int DEFAULT_TEXT_HEIGHT = 8*DEFAULT_TEXT_SCALE;

void printCentered(ILI9341_t3 &tft, char *str)
{
  int16_t x,y;
  tft.getCursor(&x,&y);
  //const char *strPtr = str.c_str();
  int16_t titleLength = tft.strPixelLen(str);
  tft.setCursor(tft.width()/2 - titleLength/2, y);
  tft.println(str);
}

void setCursorX(ILI9341_t3 &tft, int16_t xPos)
{
  int16_t x,y;
  tft.getCursor(&x, &y);
  x = xPos;
  tft.setCursor(x,y);
}

void setCursorY(ILI9341_t3 &tft, int16_t yPos)
{
  int16_t x,y;
  tft.getCursor(&x,&y);
  y = yPos;
  tft.setCursor(x,y);
}

void clearScreen(ILI9341_t3 &tft)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(DEFAULT_TEXT_SCALE);
}

#endif
