#include "Panner.h"
//#include "Keypad.h"
//#include "Trace.h"
//#include "Views.h"
#include "BatteryMonitor.h"

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

UTFT View::m_lcd(ST7735S, 11, 13, 10,  8, 9);

View *View::g_pActiveView = 0;


View::View(const char *szTitle) : m_szTitle(szTitle)
{
  
}

View::~View()
{
  
}

// Declare which fonts we will be using
extern uint8_t SmallFont[];
//extern uint8_t BigFont[];
//extern uint8_t SevenSegNumFont[];
extern uint8_t Retro8x16[];

void View::setup()
{
  m_lcd.InitLCD();
  m_lcd.clrScr();  
  m_lcd.setColor(255, 255, 255);
  m_lcd.setBackColor(0, 0, 0);
  m_lcd.setFont(Retro8x16 /* SmallFont */);
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

/**
 *  draw views title bar
 */
void View::drawTitleBar()
{
  uint8_t dX = m_lcd.getFontXsize();
  //uint8_t dY = m_lcd.getFontYsize() + 2;

  //
  // draw title bar
  //
  static const char szBefore[] = "==== ";
  m_lcd.print(szBefore, 0, 0);
  int x = dX*(sizeof(szBefore) - 1);
  m_lcd.print(m_szTitle, x, 0);
  x += dX*strlen(m_szTitle);
  static const char szAfter[] = " ====";
  m_lcd.print(szAfter, x, 0);
  
  //drawBattery(dX * 16, 4, (now/1000)%100);
  drawBattery(dX * 17, 4, g_batteryMonitor.getGauge());
}

void View::drawBattery(int16_t x, int16_t y, uint8_t iPcentFull)
{
  word c = m_lcd.getColor();
  {
    m_lcd.setColor((iPcentFull < 10) ? VGA_RED : ((iPcentFull < 80) ? VGA_WHITE : VGA_LIME));
    
    const int16_t iBatteryWidth = 20;
    const int16_t iBatteryHeight = 8;
    
    int16_t x2 = x + iBatteryWidth;
    int16_t y2 = y + iBatteryHeight;
  
    m_lcd.drawRect(x2-1, y+2, x2, y2-2); // tip!
    x2--;
    x2--;
  
    m_lcd.drawRect(x++, y++, x2--, y2--);
    x++; y++; x2--; y2--;
  
    int16_t fillWidth = x2-x;
    if(iPcentFull < 100)
      fillWidth = (fillWidth * iPcentFull)/100;
  
    m_lcd.fillRect(x, y, x + fillWidth, y2);
    if(iPcentFull < 100) {
      m_lcd.setColor(VGA_BLACK);
      m_lcd.fillRect(x+fillWidth+1, y, x2, y2);
    }
  }
  m_lcd.setColor(c);
}

/**
 *  Run View Class Implementation
 */

RunView::RunView() : View("Run")
{
}

RunView::~RunView()
{
}
 

/*
void RunView::onKeyDown(uint8_t vk)
{
  DEBUG_PRINT("RunView::onKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}

void RunView::onKeyUp(uint8_t vk)
{
  DEBUG_PRINT("RunView::onKeyUp ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}

void RunView::onLongKeyDown(uint8_t vk)
{
  DEBUG_PRINT("RunView::onLongKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}
*/

/** ThumbStick APIs where vk is one of VK_xxx */
void RunView::onThumbDown()
{
  DEBUG_PRINTLN("RunView::onThumbDown");
}

void RunView::onThumbUp()
{
  DEBUG_PRINTLN("RunView::onThumbUp");
  activate(&g_editView);
}

/*void RunView::onLongThumbDown()
{
  DEBUG_PRINTLN("RunView::onLongThumbDown");
}*/

void RunView::onThumbStickX(int16_t x)
{
  DEBUG_PRINT("RunView::onThumbStickX(");
  DEBUG_PRINTDEC(x);
  DEBUG_PRINTLN(")");

  if((-10 < x) && (x < 10)) 
  {
    ; // g_panner.setSpeed(0);
  }
  else
  {
    //float speed = g_panner.maxSpeed() * (float)x/100;
    ; // g_panner.setSpeed(speed);
  }
}

void RunView::onThumbStickY(int16_t y)
{
  DEBUG_PRINT("RunView::onThumbStickY ");
  DEBUG_PRINTDEC(y);
  DEBUG_PRINTLN("");
}

/**
 *  display Interpreter status
 */
void RunView::update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now)
{
  drawTitleBar();
  
  uint8_t dX = m_lcd.getFontXsize();
  uint8_t dY = m_lcd.getFontYsize() + 2;

  int y = 2*dY;
  int x = dX * 7;
  
  m_lcd.print("Pos:", 0, y);
  m_lcd.printNumI(lPanPos, x, y, 5);
  y += dY;
  m_lcd.print("Speed:", 0, y);
  //m_lcd.printNumF(flPanSpeed, x, y, '.', 5);
  m_lcd.printNumI((long)flPanSpeed, x, y, 5);
  y += dY;
  m_lcd.print(pLabel, 0, y);
  m_lcd.printNumI(wSecs, x, y, 5); 
}

/**
 * EditView class implementation
 */
 
EditView::EditView() : View("Edit")
{
  
}

EditView::~EditView() 
{
  
}

/** analog keyboard APIs where vk is one of VK_xxx */
//void EditView::onKeyDown(uint8_t vk);
//void EditView::onKeyUp(uint8_t vk);
//void EditView::onLongKeyDown(uint8_t vk);

/** ThumbStick APIs where vk is one of VK_xxx */
void EditView::onThumbDown() {
  
}

void EditView::onThumbUp() 
{
  activate(&g_runView); 
}

/*void EditView::onLongThumbDown() 
{
}*/

void EditView::onThumbStickX(int16_t x) {
  
}

void EditView::onThumbStickY(int16_t y) {
  
}


void EditView::update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now) 
{
  drawTitleBar();
  
}


