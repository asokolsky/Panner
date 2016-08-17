#include "Panner.h"
#include "BatteryMonitor.h"
#include <font_LiberationSans.h>
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

const uint8_t pinCS = 10;
const uint8_t pinDC = 9;

ILI9341_t3 View::m_lcd(pinCS, pinDC /* uint8_t _RST = 255, uint8_t _MOSI=11, uint8_t _SCLK=13, uint8_t _MISO=12 */);

View *View::g_pActiveView = 0;


View::View(const char *szTitle, const char *szSoftALabel, const char *szNavLabel, const char *szSoftBLabel) : 
  m_szTitle(szTitle), m_szSoftALabel(szSoftALabel), m_szNavLabel(szNavLabel), m_szSoftBLabel(szSoftBLabel)
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
}

void View::activate(View *p) 
{
  g_pActiveView = p;
  g_pActiveView->updateMaybe(millis());
}

void View::updateMaybe(unsigned long now)
{
  if(m_ulUpdated + ulUpdatePeriod < now)
  {
    update(0, 0, 0, 0, now);
    m_ulUpdated = now;
  }
}

void View::update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now)
{
  DEBUG_PRINTLN("View::update() SHOULD BE OVERWRITTEN");
}

const int16_t iBatteryWidth = 36;
//const int16_t iBatteryHeight = 16;

/**
 *  draw views title bar
 */
void View::drawTitleBar()
{
  int16_t iScreenWidth = m_lcd.width();
  //m_lcd.fillRect(0, 0, iScreenWidth, LiberationSans_20.line_space, ILI9341_BLACK);
  
  //
  // draw title bar
  //
  m_lcd.setFont(LiberationSans_20);
  m_lcd.setTextSize(1);
  m_lcd.setTextColor(ILI9341_YELLOW,ILI9341_BLACK);
  int16_t x = 2;
  int16_t y = 2;
  m_lcd.setCursor(x, y);
  m_lcd.print(m_szTitle);
  x += m_lcd.measureTextWidth(m_szTitle);
  m_lcd.fillRect(x, y, iScreenWidth - x - iBatteryWidth, m_lcd.fontLineSpace(), ILI9341_BLACK);

  drawBattery(g_batteryMonitor.getGauge());
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
  
  m_lcd.setFont(AwesomeF200_20);
  m_lcd.setTextSize(1);
  m_lcd.setTextColor(ILI9341_YELLOW,ILI9341_BLACK);

  //DEBUG_PRINT("Battery width: ");
  //DEBUG_PRINTDEC(m_lcd.measureTextWidth(szText));
  //DEBUG_PRINTLN("");  
  int16_t x = m_lcd.width() - m_lcd.measureTextWidth(szText) - 1;
  int16_t y = 0;
  
  m_lcd.setCursor(x, y);
  m_lcd.print(szText);
  
  // restore the original font
  m_lcd.setFont(LiberationSans_20);
}

/**
 * Draw bottom row with labels for soft keys
 */
void View::drawSoftLabels()
{
  int16_t iScreenWidth = m_lcd.width();
  int16_t iScreenHeight = m_lcd.height();

  int16_t x = 0;
  
  int16_t y = iScreenHeight - m_lcd.fontLineSpace();
  
  //m_lcd.fillRect(x, y, iScreenWidth, m_lcd.fontLineSpace(), ILI9341_BLACK);

  m_lcd.setTextColor(ILI9341_YELLOW,ILI9341_BLACK);
  m_lcd.setCursor(x, y);
  m_lcd.print(m_szSoftALabel);
  int16_t x1 = m_lcd.measureTextWidth(m_szSoftALabel);
  int16_t w = m_lcd.measureTextWidth(m_szNavLabel);
  int16_t x2 = (iScreenWidth - w)/2;
  m_lcd.fillRect(x1, y, x2-x1, m_lcd.fontLineSpace(), ILI9341_BLACK);
  m_lcd.setCursor(x2, y);
  m_lcd.print(m_szNavLabel);
  int16_t x3 = x2 + w;
  int16_t x4 = iScreenWidth - m_lcd.measureTextWidth(m_szSoftBLabel);
  m_lcd.fillRect(x3, y, x4-x3, m_lcd.fontLineSpace(), ILI9341_BLACK);
  m_lcd.setCursor(x4, y);
  m_lcd.print(m_szSoftBLabel);
}


void View::printKeyVal(const char *szKey, long lVal, uint16_t y)
{
  uint16_t x = m_lcd.width() / 3;
  uint16_t keyWidth = m_lcd.measureTextWidth(szKey);

  m_lcd.setTextColor(ILI9341_DARKGREY, ILI9341_BLACK);
  m_lcd.setCursor(x - keyWidth, y);
  m_lcd.print(szKey);
  m_lcd.setCursor(x, y);
  static char szSeparator[] = ": ";
  m_lcd.print(szSeparator);
  x += m_lcd.measureTextWidth(szSeparator);
  
  m_lcd.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  m_lcd.setCursor(x, y);
  m_lcd.print(lVal);
}

/**
 *  Direct Control View Class Implementation
 */


ControlView::ControlView() : View("Direct Control", "[ ]", "[<- ->]", "[Edit]")
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
    case VK_SOFTB:
      // switch to Edit view
      DEBUG_PRINTLN("ControlView::onKeyUp(VK_SOFTB): switch to Edit view");
      activate(&g_editView);
      break;
  }  
}

void ControlView::update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now)
{
  DEBUG_PRINTLN("ControlView::update()");
  drawTitleBar();
 
  uint16_t dY = m_lcd.fontLineSpace() + 2;
  uint16_t y = dY;
  printKeyVal("Pos", lPanPos, y);
  y += dY;
  printKeyVal("Speed", (long)flPanSpeed, y);
  y += dY;
  /*if(pLabel != 0)  {
    printKeyVal(pLabel, wSecs, y);    
  }*/
  drawSoftLabels();
}


/**
 * EditView class implementation
 */
 
EditView::EditView() : View("Edit", "[Sel]", "[<- ->]", "[Run]")
{
  
}

EditView::~EditView() 
{
  
}

/** analog keyboard APIs where vk is one of VK_xxx */
//void EditView::onKeyDown(uint8_t vk);
//void EditView::onKeyUp(uint8_t vk);
//void EditView::onLongKeyDown(uint8_t vk);

void EditView::update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now) 
{
  DEBUG_PRINTLN("EditView::update()");
  drawTitleBar();
  {
    // draw the content of the program here...
    uint16_t y = m_lcd.fontLineSpace() + 2;
    m_lcd.fillRect(0, y, m_lcd.width(), m_lcd.height() - (2*y), ILI9341_BLACK);
  }  
  drawSoftLabels();
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
    case VK_SEL:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_SEL): jump to the next editable field");
      break;

    case VK_LEFT:
      // decrease the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_LEFT): --");
      break;
    case VK_RIGHT:
      // increase the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_RIGHT): ++");
      break;

    case VK_UP:
      // increase the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_UP): ++");
      break;
    case VK_DOWN:
      // decrease the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_DOWN): --");
      break;

      
    case VK_SOFTB:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_SOFTB): switch to Run view");
      // switch to Edit view
      activate(&g_runView);
      // and start program execution here!
      // ....
      break;
      
    default:
      DEBUG_PRINT("EditView::onKeyUp ");
      DEBUG_PRINTDEC(vk);
      DEBUG_PRINTLN("");
  }
}

void EditView::onLongKeyDown(uint8_t vk)
{
  DEBUG_PRINT("EditView::onLongKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}

/**
 *  Run View Class Implementation
 */

RunView::RunView() : View("Run", "[ ]", "[ ]", "[Pause]")
{
}

RunView::~RunView()
{
}
 

void RunView::onKeyDown(uint8_t vk)
{
  DEBUG_PRINT("RunView::onKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}

void RunView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
    case VK_SEL:
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
      break;
    case VK_SOFTB:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_SOFTB): switch to Pause view");
      // switch to Pause view
      activate(&g_pausedRunView);
      // and suspend program execution here!
      // ....
      break;
      
    default:
      DEBUG_PRINT("RunView::onKeyUp ");
      DEBUG_PRINTDEC(vk);
      DEBUG_PRINTLN("");
  }
}

void RunView::onLongKeyDown(uint8_t vk)
{
  DEBUG_PRINT("RunView::onLongKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");

}

/**
 *  display Interpreter status
 */
void RunView::update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now)
{
  DEBUG_PRINTLN("RunView::update()");
  
  drawTitleBar();
  {
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
  drawSoftLabels();
}

/**
 *  Run View Class Implementation
 */

PausedRunView::PausedRunView() : View("Paused", "[ ]", "[ ]", "[Run]")
{
}

PausedRunView::~PausedRunView()
{
}
 

void PausedRunView::onKeyDown(uint8_t vk)
{
  DEBUG_PRINT("PausedRunView::onKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}

void PausedRunView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
    case VK_SEL:
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
      break;
      
    case VK_SOFTB:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_SOFTB): switch to Run view");
      // switch to Edit view
      activate(&g_runView);
      // and resume program execution here!
      // ....
      break;
      
    default:
      DEBUG_PRINT("PausedRunView::onKeyUp ");
      DEBUG_PRINTDEC(vk);
      DEBUG_PRINTLN("");
  }
}

void PausedRunView::onLongKeyDown(uint8_t vk)
{
  DEBUG_PRINT("RunView::onLongKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");

}

/**
 *  display Interpreter status
 */
void PausedRunView::update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now)
{
  DEBUG_PRINTLN("PausedRunView::update()");
  
  drawTitleBar();
  {
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
  drawSoftLabels();
}


