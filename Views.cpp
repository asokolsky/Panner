#include "Panner.h"
#include "BatteryMonitor.h"
#include <font_LiberationSans.h>
#include <font_AwesomeF000.h>
#include <font_AwesomeF100.h>
#include <font_AwesomeF200.h>

/**
 *  +------------------------------
 *  |CurPos:   -12345
 *  |CurSpeed: 123
 *  |MaxSpeed: 1234
 *  |
 *  |MaxLeft:  -123456*  
 *  |MaxRight: 1234
 *  |MaxSpeed: 123
 *  |
 *  +------------------------------
 */

/**
 * Globals: views
 */
Display View::m_lcd;
View *View::g_pActiveView = 0;
RECT View::g_rectClient;

ControlView g_controlView;
EditView g_editView;
RunView g_runView;
PausedRunView g_pausedRunView;
AboutView g_aboutView;

const int16_t iBatteryWidth = 36;
//const int16_t iBatteryHeight = 16;

const int16_t iTitleBarHeight = 26;
const int16_t iBottomBarHeight = 35;
const int16_t iButtonCornerRadius = 4;
const uint16_t uButtonBorderColor = ILI9341_DARKGREY;

/**
 * Class View
 */
View::View(const char *szTitle, 
           const ILI9341_t3_font_t &fontSoftA, const char *szSoftALabel, 
           const ILI9341_t3_font_t &fontNav, const char *szNavLabel, 
           const ILI9341_t3_font_t &fontSoftB, const char *szSoftBLabel) :
  m_szTitle(szTitle), 
  m_fontSoftA(fontSoftA), m_szSoftALabel(szSoftALabel), 
  m_fontNav(fontNav), m_szNavLabel(szNavLabel), 
  m_fontSoftB(fontSoftB), m_szSoftBLabel(szSoftBLabel)
{
  
}

View::~View()
{
  
}

// Declare which fonts we will be using
//extern uint8_t SmallFont[];

void View::setup()
{
  m_lcd.begin();
  m_lcd.fillScreen(ILI9341_BLACK);
  m_lcd.setRotation(1);
  m_lcd.setTextWrap(false);
}

void View::activate(View *p) 
{
  g_pActiveView = p;
  g_pActiveView->update(0, 0, 0, 0, millis());
}

void View::updateMaybe(unsigned long now)
{
  if(m_ulToUpdate > now)
  {
    m_ulToUpdate = now + ulUpdatePeriod;
    update(0, 0, 0, 0, now);
  }
}

/** 
 * entire screen redraw 
 */
void View::update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now)
{
  g_rectClient.left = 0;
  g_rectClient.right = m_lcd.width();
  g_rectClient.top = drawTitleBar();
  g_rectClient.bottom = m_lcd.height() - drawSoftLabels();
  
  //
  // try to protect non-client area  
  //
  m_lcd.setClipRect(g_rectClient.left, g_rectClient.top, g_rectClient.right, g_rectClient.bottom);
  //
  // set defaults for use in the client area
  //
  m_lcd.setFont(LiberationSans_18);  
  m_lcd.setTextSize(1);
  m_lcd.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  m_lcd.setCursor(g_rectClient.left, g_rectClient.top);
  updateClient(lPanPos, flPanSpeed, pLabel, wSecs, now);
  //
  m_ulToUpdate = now + ulUpdatePeriod;
}

/** 
 *  redraw client area only, not including title and bottom bar 
 */
void View::updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now)
{
  DEBUG_PRINTLN("View::updateClient() SHOULD BE OVERWRITTEN");
}

/**
 *  draws the title bar
 *  returns title bar height
 */
int16_t View::drawTitleBar()
{
  int16_t iScreenWidth = m_lcd.width(); 
  //
  // draw the title itself
  //
  m_lcd.setFont(LiberationSans_18);
  m_lcd.setTextSize(1);
  m_lcd.setTextColor(ILI9341_YELLOW,ILI9341_BLACK);
  int16_t x = 2;
  int16_t y = 2;
  int16_t w = m_lcd.measureTextWidth(m_szTitle);
  m_lcd.setCursor(x, y);
  m_lcd.setClipRect(x, y, x + w, y + m_lcd.fontLineSpace());
  m_lcd.print(m_szTitle);
  m_lcd.setClipRect();
  x += w;
  // clear the space between end of the title and the battery icon
  m_lcd.fillRect(x, y, iScreenWidth - x - iBatteryWidth, m_lcd.fontLineSpace(), ILI9341_BLACK);

  drawBattery(g_batteryMonitor.getGauge());  

  return y + m_lcd.fontLineSpace();
}

/**
 *  draw the battery icon in the top right corner of the screen
 */
void View::drawBattery(uint8_t iPcentFull)
{
  // map iPcentFull into '@', 'A', 'B', 'C', 'D'
  char szText[] = "@";
  if(iPcentFull > 80)
    szText[0] = '@';
  else if(iPcentFull > 60)
    szText[0] = 'A';
  else if(iPcentFull > 40)
    szText[0] = 'B';
  else if(iPcentFull > 20)
    szText[0] = 'C';
  else
    szText[0] = 'D';
  
  m_lcd.setFont(AwesomeF200_18);
  m_lcd.setTextSize(1);
  m_lcd.setTextColor((iPcentFull < 10) ? ILI9341_RED : ((iPcentFull > 90) ? ILI9341_GREEN: ILI9341_YELLOW),
                     ILI9341_BLACK);

  //DEBUG_PRINT("Battery width: ");
  //DEBUG_PRINTDEC(m_lcd.measureTextWidth(szText));
  //DEBUG_PRINTLN("");  
  int16_t x = m_lcd.width() - m_lcd.measureTextWidth(szText) - 1;
  int16_t y = 0;
  
  m_lcd.setCursor(x, y);
  m_lcd.print(szText);
}

void View::drawButton(int16_t x, int16_t y, int16_t w, int16_t h, const ILI9341_t3_font_t &font, const char *szLabel)
{ 
  m_lcd.drawRoundRect(x, y, w, h, iButtonCornerRadius, uButtonBorderColor);
  x += iButtonCornerRadius;
  int16_t y0 = y;
  y += iButtonCornerRadius;
  w -= 2*iButtonCornerRadius;
  int16_t h0 = h;
  h -= 2*iButtonCornerRadius;
  m_lcd.setClipRect(x, y, x + w, y+h);
  if(szLabel == 0)
  {
    m_lcd.fillRect(x, y, w, h, ILI9341_BLACK); // clear the entire button face
  }
  else
  {
    m_lcd.setFont(font);  
    int16_t tw = m_lcd.measureTextWidth(szLabel);
    int16_t th = m_lcd.measureTextHeight(szLabel);
    int16_t gw = (w-tw)/2;
    m_lcd.fillRect(x, y, gw, h, ILI9341_BLACK); // clear the entire button face
    x += gw;
    m_lcd.setCursor(x, y0 + ((h0 - th)/2));
    m_lcd.print(szLabel);
    x += tw;
    m_lcd.fillRect(x, y, gw, h, ILI9341_BLACK); // clear the entire button face
  }  
  m_lcd.setClipRect();
}

/**
 * Draw bottom row with labels for soft keys
 * returns the bar height
 */
int16_t View::drawSoftLabels()
{
  m_lcd.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  //m_lcd.setFont(m_fontSoftA);
  int16_t iButtonWidth = (m_lcd.width() / 3) - 12;
  int16_t iButtonHeight = iBottomBarHeight; // m_lcd.fontLineSpace() + 2*iButtonCornerRadius;
  int16_t y = m_lcd.height() - iButtonHeight;
  drawButton(0, y, iButtonWidth, iButtonHeight, m_fontSoftA, m_szSoftALabel);
  drawButton((m_lcd.width() - iButtonWidth)/2, y, iButtonWidth, iButtonHeight, m_fontNav, m_szNavLabel);
  drawButton(m_lcd.width() - iButtonWidth, y, iButtonWidth, iButtonHeight, m_fontSoftB, m_szSoftBLabel);
  return iButtonHeight;
}

/**
 * Print Key: Val
 * with Key in ILI9341_DARKGREY
 * and Val in ILI9341_WHITE
 * and centered around ':'
 */
void View::printKeyVal(const char *szKey, long lVal, uint16_t y)
{
  uint16_t x = m_lcd.width() / 3;
  uint16_t w = m_lcd.measureTextWidth(szKey);
  uint16_t iFontLineSpace = m_lcd.fontLineSpace();
    
  m_lcd.fillRect(0, y, x - w, iFontLineSpace, ILI9341_BLACK);
  m_lcd.setTextColor(ILI9341_DARKGREY, ILI9341_BLACK);
  m_lcd.setCursor(x - w, y);
  m_lcd.print(szKey);
  m_lcd.setCursor(x, y);
  static const char szSeparator[] = ": ";
  m_lcd.print(szSeparator);
  x += m_lcd.measureTextWidth(szSeparator);  
  m_lcd.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  m_lcd.setCursor(x, y);
  char szText[80];
  sprintf(szText, "%ld", lVal);
  m_lcd.print(szText);
  x += m_lcd.measureTextWidth(szText);
  w = m_lcd.width();
  if(x < w)
    m_lcd.fillRect(x, y, w, iFontLineSpace, ILI9341_BLACK);
}

/**
 *  print text left-aligned in the client area using current font
 */
void View::printTextLeft(const char *szText, uint16_t y, const ILI9341_t3_font_t *pFont)
{
  const ILI9341_t3_font_t *pOldFont = 0;
  if(pFont != 0) {
    pOldFont = m_lcd.getFont();
    m_lcd.setFont(*pFont);
  }
  int16_t x = g_rectClient.left;
  m_lcd.setCursor(x, y);
  m_lcd.print(szText);
  int16_t x1 = x + m_lcd.measureTextWidth(szText);
  if(x1 < g_rectClient.right)
    m_lcd.fillRect(x1, y, g_rectClient.width() - x1, m_lcd.fontLineSpace(), ILI9341_BLACK);
  if(pOldFont != 0)
    m_lcd.setFont(*pOldFont);
}

/**
 *  print text centered in the client area using current font
 */
void View::printTextCenter(const char *szText, uint16_t y, const ILI9341_t3_font_t *pFont, int16_t *pDY)
{
  const ILI9341_t3_font_t *pOldFont = 0;
  if(pFont != 0) {
    pOldFont = m_lcd.getFont();
    m_lcd.setFont(*pFont);
  }
  int16_t x = g_rectClient.left;
  int16_t tw = m_lcd.measureTextWidth(szText);
  int16_t x1 = x + (g_rectClient.width() - tw)/2;
  int16_t dY = m_lcd.fontLineSpace();
  m_lcd.fillRect(g_rectClient.left, y, x1-x, dY, ILI9341_BLACK);
  m_lcd.setCursor(x1, y);
  m_lcd.print(szText);
  x1 += tw;
  if(x1 < g_rectClient.right)
    m_lcd.fillRect(x1, y, g_rectClient.right - x1, dY, ILI9341_BLACK);
  if(pDY != 0)
    *pDY = dY;    
  if(pOldFont != 0)
    m_lcd.setFont(*pOldFont);
}


void View::onKeyDown(uint8_t vk) {
}
void View::onLongKeyDown(uint8_t vk) {
}
void View::onKeyUp(uint8_t vk) {
}

/**
 *  Direct Control View Class Implementation
 */


ControlView::ControlView() : View("Direct Control", 
  AwesomeF000_16, "Z",  // i
  AwesomeF000_16, "\x7E", 
  LiberationSans_18, "Edit")
{
}

ControlView::~ControlView()
{
}
  
/** analog keyboard APIs where vk is one of VK_xxx */
void ControlView::onKeyDown(uint8_t vk)
{
  switch(vk) {
    case VK_LEFT:
      // start pan left
      DEBUG_PRINTLN("ControlView::onKeyDown(VK_LEFT): start pan left");
      break;
    case VK_RIGHT:
      // start pan right
      DEBUG_PRINTLN("ControlView::onKeyDown(VK_RIGHT): start pan right");
      break;
  }
}

void ControlView::onLongKeyDown(uint8_t vk)
{
  switch(vk) {
    case VK_LEFT:
      // start fast pan left
      DEBUG_PRINTLN("ControlView::onLongKeyDown(VK_LEFT): start fast pan left");
      break;
    case VK_RIGHT:
      // start fast pan right
      DEBUG_PRINTLN("ControlView::onLongKeyDown(VK_RIGHT): start fast pan right");
      break;
  }
}

void ControlView::onKeyUp(uint8_t vk)
{
  switch(vk) {
    case VK_LEFT:
      // stop pan left
      DEBUG_PRINTLN("ControlView::onKeyUp(VK_LEFT): stop pan");
      break;
    case VK_RIGHT:
      // stop pan right
      DEBUG_PRINTLN("ControlView::onKeyUp(VK_RIGHT): stop pan");
      break;
    case VK_SOFTA:
      // switch to About view
      DEBUG_PRINTLN("ControlView::onKeyUp(VK_SOFTA): switch to About view");
      activate(&g_aboutView);
      break;
    case VK_SOFTB:
      // switch to Edit view
      DEBUG_PRINTLN("ControlView::onKeyUp(VK_SOFTB): switch to Edit view");
      activate(&g_editView);
      break;
  }  
}

void ControlView::updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now)
{
  DEBUG_PRINTLN("ControlView::updateClient()");
 
  uint16_t y = m_lcd.getCursorY();
  uint16_t dY = m_lcd.fontLineSpace();
  m_lcd.fillRect(0, y, m_lcd.width(), dY, ILI9341_BLACK);
  y += dY;
  printKeyVal("Pos", lPanPos, y);
  y += dY;
  printKeyVal("Speed", (long)flPanSpeed, y);
  y += dY;
  /*if(pLabel != 0)  {
    printKeyVal(pLabel, wSecs, y);    
  }*/
  m_lcd.fillRect(0, y, m_lcd.width(), m_lcd.height(), ILI9341_BLACK);
}


/**
 * EditView class implementation
 */
EditView::EditView() : View("Edit", 
  LiberationSans_18, "Sel/Ctrl", 
  AwesomeF000_16, "\x7D", // up/down
  AwesomeF000_16, "K")  // Run
{
  
}

EditView::~EditView() 
{
  
}

/** analog keyboard APIs where vk is one of VK_xxx */
void EditView::updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now) 
{
  DEBUG_PRINTLN("EditView::updateClient()");
  // draw the content of the program here...
  uint16_t y = m_lcd.fontLineSpace() + 2;
  m_lcd.fillRect(0, y, m_lcd.width(), m_lcd.height() - (2*y), ILI9341_BLACK);
}

void EditView::onKeyDown(uint8_t vk)
{
  DEBUG_PRINT("EditView::onKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}

void EditView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {

    /*case VK_LEFT:
      // decrease the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_LEFT): --");
      break;
    case VK_RIGHT:
      // increase the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_RIGHT): ++");
      break;*/

    case VK_UP:
      // increase the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_UP): ++");
      break;
    case VK_DOWN:
      // decrease the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_DOWN): --");
      break;

      
    case VK_SEL:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_SEL): jump to the next editable field");
      break;
    case VK_SOFTA:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_SOFTA): jump to the next editable field");
      break;
      
    case VK_SOFTB:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_SOFTB): switch to Run view");
      // switch to Edit view
      activate(&g_runView);
      // and start program execution here!
      // ....
      break;
      
    /*default:
      DEBUG_PRINT("EditView::onKeyUp ");
      DEBUG_PRINTDEC(vk);
      DEBUG_PRINTLN("");*/
  }
}

void EditView::onLongKeyDown(uint8_t vk)
{
  switch(vk)
  {
    case VK_SOFTA:
      DEBUG_PRINTLN("EditView::onLongKeyDown(VK_SOFTA): back to direct control");
      activate(&g_controlView);
      // and start direct control here!
      // ....
      break;
    /*default:
      DEBUG_PRINT("EditView::onLongKeyDown ");
      DEBUG_PRINTDEC(vk);
      DEBUG_PRINTLN("");*/
  }
}

/**
 *  Run View Class Implementation
 */
RunView::RunView() : View("Run", 
  AwesomeF000_16, "L", // Pause
  LiberationSans_18, 0, 
  AwesomeF000_16, "M") // Stop
{
}

RunView::~RunView()
{
}
 
void RunView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
    /*case VK_SEL:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_SEL)");
      break;
    case VK_LEFT:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_LEFT)");
      break;     
    case VK_RIGHT:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_RIGHT)");
      break;
    case VK_UP:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_UP)");
      break;      
    case VK_DOWN:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_DOWN)");
      break;*/
    case VK_SOFTA:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_SOFTA): switch to Pause view");
      // switch to Pause view
      activate(&g_pausedRunView);
      // and suspend program execution here!
      // ....
      break;
    case VK_SOFTB:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_SOFTB): stop and back to edit");
      // stop execution here!
      // switch to Edit view
      activate(&g_editView);
      // ....
      break;
      
    /*default:
      DEBUG_PRINT("RunView::onKeyUp ");
      DEBUG_PRINTDEC(vk);
      DEBUG_PRINTLN("");*/
  }
}

/*void RunView::onLongKeyDown(uint8_t vk)
{
  DEBUG_PRINT("RunView::onLongKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");

}*/

/**
 *  display Interpreter status
 */
void RunView::updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now)
{
  DEBUG_PRINTLN("RunView::updateClient()");
  
  uint16_t dY = m_lcd.fontLineSpace() + 2;
  uint16_t y = 2*dY;
  printKeyVal("Pos", lPanPos, y);
  y += dY;
  printKeyVal("Speed", (long)flPanSpeed, y);
  y += dY;
  if(pLabel != 0)  {
    printKeyVal(pLabel, wSecs, y);    
  }
}

/**
 *  Run View Class Implementation
 */
PausedRunView::PausedRunView() : View("Paused", 
  AwesomeF000_16, "K",  // Run
  LiberationSans_18, 0, 
  AwesomeF000_16, "M") // Stop
{
}

PausedRunView::~PausedRunView()
{
}
 
void PausedRunView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
    /*case VK_SEL:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_SEL)");
      break;
    case VK_LEFT:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_LEFT)");
      break;
    case VK_RIGHT:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_RIGHT)");
      break;
    case VK_UP:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_UP)");
      break;
    case VK_DOWN:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_DOWN)");
      break;*/
      
    case VK_SOFTA:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_SOFTA): switch to Run view");
      // resume program execution here!
      // switch to Run view
      activate(&g_runView);
      // ....
      break;
    case VK_SOFTB:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_SOFTB): stop and switch to Run view");
      // stop program execution here!
      // switch to Edit view
      activate(&g_editView);
      // ....
      break;
      
    /*default:
      DEBUG_PRINT("PausedRunView::onKeyUp ");
      DEBUG_PRINTDEC(vk);
      DEBUG_PRINTLN("");*/
  }
}


/**
 *  display Interpreter status
 */
void PausedRunView::updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now)
{
  DEBUG_PRINTLN("PausedRunView::updateClient()");
  
  uint16_t dY = m_lcd.fontLineSpace() + 2;
  uint16_t y = 2*dY;
  printKeyVal("Pos", lPanPos, y);
  y += dY;
  printKeyVal("Speed", (long)flPanSpeed, y);
  y += dY;
  if(pLabel != 0)  {
    printKeyVal(pLabel, wSecs, y);    
  }
}


/**
 *  About View Class Implementation
 */
const char *AboutView::m_lines[] = {
  "",
  "Panner v0.1",
  "(c) 2015-2016 Alex Sokolsky",
  "",
  "Thank you to:",
  "Dean Blackketter for a fork of ILI9341_t3",
  "Paul Stoffregen for ILI9341_t3",
  "Paul Stoffregen for Teensy 3.x",
  "Mike McCauley for AccelStepper",
  "",
  "This code is licensed under the terms",
  "of the GNU Public License, verison 2."
};


int16_t AboutView::m_iLines = sizeof(m_lines)/ sizeof(m_lines[0]);

 
AboutView::AboutView() : View("About", 
  AwesomeF100_16, "d",
  AwesomeF000_16, "\x7D", // up/down
  AwesomeF100_16, "e")
{
}

AboutView::~AboutView()
{
}
 
void AboutView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
    case VK_DOWN:
      DEBUG_PRINTLN("AboutView::onKeyUp(VK_DOWN)");
      m_iFirstDisplayedLine++;
      if(m_iFirstDisplayedLine >= m_iLines)
        m_iFirstDisplayedLine = m_iLines - 1;
      break;
    case VK_UP:
      DEBUG_PRINTLN("AboutView::onKeyUp(VK_UP)");
      m_iFirstDisplayedLine--;      
      if(m_iFirstDisplayedLine < 0)
        m_iFirstDisplayedLine = 0;
      break;
      
    case VK_SOFTA:
    case VK_SOFTB:
      DEBUG_PRINTLN("AboutView::onKeyUp(VK_SOFTB): back to control view");
      activate(&g_controlView);
      // ....
      break;      
  }
}


/**
 *  display About info in a scrollable line list
 */
void AboutView::updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now)
{
  DEBUG_PRINTLN("AboutView::updateClient()");

  int16_t x = g_rectClient.left;
  int16_t y = g_rectClient.top;
  int16_t iClientWidth = g_rectClient.width();
  if(m_iFirstDisplayedLine > 0)
  {
    int16_t dY = 0;
    printTextCenter("\x06", y, &AwesomeF100_14, &dY);
    y += dY;
  }
  m_lcd.setFont(LiberationSans_14);
  int16_t iFontLineSpace = m_lcd.fontLineSpace();
  for(int i = m_iFirstDisplayedLine; i <  (sizeof(m_lines)/ sizeof(m_lines[0])); i++)
  {
    if(m_lines[i] == 0)
      break;
    if(y + iFontLineSpace > g_rectClient.bottom)
    {
      int16_t dY = 0;
      printTextCenter("\x07", y, &AwesomeF100_14, &dY);
      y += dY;
      break;
    }
    printTextLeft(m_lines[i], y);
    y += iFontLineSpace;
  }
  if(y < g_rectClient.bottom)
    m_lcd.fillRect(x, y, iClientWidth, g_rectClient.bottom - y, ILI9341_BLACK);    
}


