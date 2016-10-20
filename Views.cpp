#include "Panner.h"
#include "BatteryMonitor.h"
#include <font_LiberationSans.h>
//#include <font_AwesomeF000.h>
//#include <font_AwesomeF100.h>
#include <font_AwesomeF200.h>

/**
 * Generic Views (Top Level Windows) Implementation
 */


/**
 * Globals: views
 */
View *View::g_pActiveView = 0;
View *View::g_pPreviousView = 0;
Stepper *View::g_pPanner = 0;
unsigned long View::m_ulToUpdate = 0;



const int16_t iBatteryWidth = 36;
//const int16_t iBatteryHeight = 16;

const int16_t iTitleBarHeight = 30; // 27;
const int16_t iBottomBarHeight = 35;

const char szCancel[] = "Cancel";
const char szOK[] = "OK";
const char szConfirmation[] = "Confirmation";

/**
 * Class View
 */
View::View(const char *szTitle, 
           const ILI9341_t3_font_t *fontSoftA, const char *szSoftALabel, 
           const ILI9341_t3_font_t *fontNav, const char *szNavLabel, 
           const ILI9341_t3_font_t *fontSoftB, const char *szSoftBLabel) :
  m_szTitle(szTitle), 
  m_fontSoftA(fontSoftA), m_szSoftALabel(szSoftALabel), 
  m_fontNav(fontNav), m_szNavLabel(szNavLabel), 
  m_fontSoftB(fontSoftB), m_szSoftBLabel(szSoftBLabel)
{
  if(m_fontSoftA == 0) m_fontSoftA = &LiberationSans_16;
  if(m_fontSoftB == 0) m_fontSoftB = &LiberationSans_16;
  if(m_fontNav == 0) m_fontNav = &LiberationSans_16;
  m_position.top = 0;
  m_position.left = 0;
  m_position.bottom = 240; // m_lcd.width(); // the order is important! 
  m_position.right = 320; // m_lcd.height();
  setPosition(m_position);  
}

/**
 * Called once to set things up.
 */
void View::setup()
{
  m_lcd.setup();
  g_pPanner = g_ci.getPanner();
  if(g_pPanner != 0) {
    g_pPanner->setMaxSpeed(g_settings.m_uPannerMaxSpeed);
    g_pPanner->setAcceleration(g_settings.m_uPannerAcceleration);
  }

}

/** 
 * to be called from the main loop on the active view.  Do nothing by default. Return TRUE to update display
 */
boolean View::loop(unsigned long now)
{
  return false;
}

void View::activate(View *p) 
{
  if(g_pActiveView != 0)
  {
    g_pActiveView->onDeActivate(p);
  }
  if(p != 0)
  {
    p->onActivate(g_pPreviousView = g_pActiveView);
    g_pActiveView = p;
    p->update(millis());
  }
  else
  {
    DEBUG_PRINTLN("BUMMER! View::activate(0!!!)");   
  }
}

void View::onDeActivate(View *pNewActive)
{
  DEBUG_PRINTLN("View::onDeActivate");
}

void View::onActivate(View *pPrevActive)
{
  DEBUG_PRINTLN("View::onActivate");
  m_lcd.resetClipRect();
  RECT rPos;
  rPos.top = 0;
  rPos.left = 0;
  rPos.right = m_lcd.width();
  rPos.bottom = m_lcd.height();
  setPosition(rPos); // done in the constructor but let's reiterate
  // erase the entire background
  m_bEraseBkgnd = true;
}


void View::updateMaybe(unsigned long now)
{
  if(now < m_ulToUpdate)
    return;
  m_ulToUpdate = now + ulUpdatePeriod;
  update(now);
}

/** 
 *  m_position was just changed.  
 *  Default implementation updates m_rectClient.
 *  If there is exactly one child - stretch it to cover the client.
 *  Derived class can overwrite this behaviour in oPosition or onActivate
 */
void View::onPosition()
{
  m_rectClient = m_position;
  m_rectClient.top += iTitleBarHeight;
  m_rectClient.bottom -= iBottomBarHeight;
  if(m_zChildren.size() == 1)
    m_zChildren[0]->setPosition(m_rectClient);
}


/** 
 * Entire screen redraw
 * Update non-clietn area and then call a virtual update of the client in a safe sandbox
 */
void View::update(unsigned long now)
{
  //DUMP("View::update()");
  if(m_bEraseBkgnd)
  {
    RECT rFill = m_position;
    rFill.bottom -= iBottomBarHeight;
    m_lcd.fillRect(rFill); //, ILI9341_OLIVE);
  }
  drawTitleBar();
  drawSoftLabels(m_bEraseBkgnd);
  m_bEraseBkgnd = false;
  //
  // try to protect non-client area  
  // 
  m_lcd.setClipRect(m_rectClient);
  
  //
  // set defaults for use in the client area
  //
  m_lcd.setFont(LiberationSans_16);  
  m_lcd.setTextSize(1);
  m_lcd.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  m_lcd.setCursor(m_rectClient.left, m_rectClient.top);
  
  updateClient(now);
  //
  m_ulToUpdate = now + ulUpdatePeriod;
}

/** 
 *  redraw client area only, not including title and bottom bar 
 */
void View::updateClient(unsigned long now)
{
  DEBUG_PRINTLN("View::updateClient()");
  // entire background erase - does the job but blinks!
  // m_lcd.fillRect(m_rectClient, ILI9341_BLUE);
  // redraw children!
  for(size_t i = 0; i < m_zChildren.size(); i++)
    m_zChildren[i]->draw();
}

/**
 *  draws the title bar
 */
void View::drawTitleBar()
{
  // DEBUG_PRINT("View::drawTitleBar("); DEBUG_PRINT(m_szTitle);  DEBUG_PRINTLN(")");
  RECT r;
  r.top = m_position.top + 2;
  r.bottom = 0;
  r.left = m_position.left + 2;
  r.right = m_position.right - iBatteryWidth;
  m_lcd.printText(m_szTitle, ILI9341_YELLOW, ILI9341_BLACK, r, Display::haLeft, Display::vaTop, &LiberationSans_18);
  
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
  
  m_lcd.setFont(AwesomeF200_18);
  m_lcd.setTextSize(1);
  m_lcd.setTextColor((iPcentFull < 10) ? ILI9341_RED : ((iPcentFull > 90) ? ILI9341_GREEN: ILI9341_YELLOW),
                     ILI9341_BLACK);

  //DEBUG_PRINT("Battery width: "); DEBUG_PRINTDEC(m_lcd.measureTextWidth(szText)); DEBUG_PRINTLN("");
  m_lcd.setCursor(m_lcd.width() - m_lcd.measureTextWidth(szText) - 1, 0);
  m_lcd.print(szText);
}

/**
 * Draw bottom row with labels for soft keys
 */
void View::drawSoftLabels(bool bEraseBkgnd)
{
  int16_t iButtonWidth = (m_lcd.width() / 3) - 12;
  int16_t iButtonHeight = iBottomBarHeight;
  RECT rButton;
  rButton.top = m_lcd.height() - iButtonHeight;
  rButton.bottom = rButton.top + iButtonHeight;
  rButton.left = 0;
  rButton.right = iButtonWidth; 
  m_lcd.drawButton(rButton, m_fontSoftA, m_szSoftALabel, bEraseBkgnd);
  rButton.left = (m_lcd.width() - iButtonWidth)/2;
  rButton.right = rButton.left + iButtonWidth;
  m_lcd.drawButton(rButton, m_fontNav, m_szNavLabel, bEraseBkgnd);
  rButton.left = m_lcd.width() - iButtonWidth;
  rButton.right = rButton.left + iButtonWidth;
  m_lcd.drawButton(rButton, m_fontSoftB, m_szSoftBLabel, bEraseBkgnd);
}

/** dummy defaults, children to overwrite */
bool View::onKeyDown(uint8_t vk) {
  return false;
}
bool View::onKeyAutoRepeat(uint8_t vk) {
  return false;
}
bool View::onLongKeyDown(uint8_t vk) {
  return false;
}
bool View::onKeyUp(uint8_t vk) {
  return false;
}

/** updateClient implementation for Run or Paused view */
void View::updateClientRunOrPaused(unsigned long now, bool bExtendedInfo, const char *pMsg)
{
  uint16_t x = m_rectClient.width() / 4;             // position of the first ':'
  uint16_t y = m_rectClient.top;
  y += m_lcd.fontLineSpace(); 
  
  printKeyVal(x, y, "Pan", g_pPanner->currentPosition(), false, "Speed", (long)g_pPanner->speed());
  y += m_lcd.fontLineSpace() + 2;
  printKeyVal(x, y, "To", g_pPanner->targetPosition());
  y += m_lcd.fontLineSpace() + 2;

  if(bExtendedInfo)
  {   
    const char *pLabel = 0;
    unsigned wSecs = g_ci.getBusySeconds(now);
    if(g_ci.isResting()) {
      pLabel = "Rest";
      wSecs = g_ci.getRestSeconds(now);
    } else if(g_ci.isWaitingForCompletion()) {
      pLabel = "Wait";
      wSecs = g_ci.getWaitSeconds(now);
    } else if(g_ci.isPaused()) {
      pLabel = "Paused";
    } else {
      pLabel = "Stopped";
    }  
    printKeyVal(x, y, pLabel, wSecs);
    y += m_lcd.fontLineSpace();
  }
  if((pMsg != 0) && (pMsg[0] != '\0'))
  {
    RECT r = m_rectClient;
    r.top = r.bottom - m_lcd.fontLineSpace();    
    m_lcd.printText(pMsg, ILI9341_DARKGREY, ILI9341_BLACK, r, Display::haLeft, Display::vaCenter, 0, false);      
    //printTextLeft(pMsg, y);
    //y += m_lcd.fontLineSpace();
  } 
  //g_pPanner->DUMP();
}

#ifdef DEBUG
void View::DUMP(const char *szText /*= 0*/) const
{
  Widget::DUMP(szText);
  DEBUG_PRINT("View@"); DEBUG_PRINTDEC((int)this); DEBUG_PRINT(" m_szTitle="); DEBUG_PRINTLN(m_szTitle); 
}
#endif

/**
 *  ModalDialog Class Implementation
 */
ModalDialog::ModalDialog(const char *szTitle, 
  const ILI9341_t3_font_t *fontSoftA, const char *szSoftALabel, 
  const ILI9341_t3_font_t *fontNav, const char *szNavLabel, 
  const ILI9341_t3_font_t *fontSoftB, const char *szSoftBLabel) :
  View(szTitle, fontSoftA, szSoftALabel, fontNav, szNavLabel, fontSoftB, szSoftBLabel),
  m_uType(MB_OKCANCEL)  
{
  
}

ModalDialog::ModalDialog(const char *szTitle, uint16_t uType) :
  View(szTitle, 0, szCancel, 0, 0, 0, szOK),
  m_uType(uType)
{
  switch(uType) 
  {
    case MB_OK:
      m_szSoftALabel = 0;
      break;
    case MB_OKCANCEL:
      break;
    /*
case MB_ABORTRETRYIGNORE
case MB_YESNOCANCEL
case MB_YESNO      
case case MB_RETRYCANCEL
case MB_CANCELTRYCONTINUE
case MB_HELP
    */
  }  
}

/**
 *  draws the title bar
 */
void ModalDialog::drawTitleBar()
{
  // DEBUG_PRINT("ModalDialog::drawTitleBar("); DEBUG_PRINT(m_szTitle); DEBUG_PRINTLN(")");
  m_lcd.drawLine(m_position.left, m_position.top, m_position.right, m_position.top, ILI9341_DARKGREY);

  RECT rLoc = m_position;
  rLoc.top += 3;
  rLoc.bottom = 0;
  m_lcd.printText(m_szTitle, ILI9341_YELLOW, ILI9341_BLACK, rLoc, Display::haCenter, Display::vaTop, &LiberationSans_18);
}

bool ModalDialog::onKeyUp(uint8_t vk)
{
  DEBUG_PRINTLN("ModalDialog::onKeyUp");
  switch(vk) {
    case VK_SOFTA:
      m_iRes = IDCANCEL;
      break;
    case VK_SOFTB:
      // Affirms!
      m_iRes = IDOK;
      break;
    default:
      return false;
  }  
  // Close thge dialog and activate parent
  activate(m_zParent);
  return true;
}

/**
 *  Position the modal dialog so that it does NOT obscure its parent
 */
void ModalDialog::onActivate(View *pPrevActive)
{
  m_iRes = IDUNKNOWN;
  m_zParent = pPrevActive;
  if(pPrevActive != 0) {
    RECT rPos = pPrevActive->m_position;
    rPos.top += iTitleBarHeight;
    setPosition(rPos);
  }
  // erase the entire background
  m_bEraseBkgnd = true;
  //RECT rFill = m_position;
  //rFill.bottom -= iBottomBarHeight;
  //m_lcd.fillRect(rFill); //, ILI9341_DARKCYAN);
}

/**
 *  MessageBox Class implementation
 */
MessageBox::MessageBox(const char *szTitle, uint16_t uType /* = MB_OK */) :
  ModalDialog(szTitle, uType)
{
}
    
void MessageBox::updateClient(unsigned long now)
{
  m_lcd.printText(m_strMessage.c_str(), m_lcd.getTextColor(), ILI9341_BLACK, m_rectClient, Display::haCenter, Display::vaCenter, 0, false);
}

