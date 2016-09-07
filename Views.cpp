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


const int16_t iBatteryWidth = 36;
//const int16_t iBatteryHeight = 16;

const int16_t iTitleBarHeight = 27;
const int16_t iBottomBarHeight = 35;
const int16_t iButtonCornerRadius = 4;
const uint16_t uButtonBorderColor = ILI9341_DARKGREY;
const uint16_t uButtonLabelColor = ILI9341_YELLOW;
const uint16_t uButtonFaceColor = ILI9341_BLACK; // ILI9341_DARKGREEN;


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
  setPosition(0, 0, m_lcd.height(), m_lcd.width());  // the order is important!
}

/**
 * Called once to set things up.
 */
void View::setup()
{
  m_lcd.setup();
  g_pPanner = g_ci.getPanner();
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
  /*RECT rFill = m_position;
  rFill.bottom -= iBottomBarHeight;
  m_lcd.fillRect(rFill, ILI9341_BLACK);*/
}

void View::onActivate(View *pPrevActive)
{
  DEBUG_PRINTLN("View::onActivate");
  m_lcd.resetClipRect();
  setPosition(0, 0, m_lcd.width(), m_lcd.height()); // done in the constructor but let's reiterate
  // erase the entire background
  RECT rFill = m_position;
  rFill.bottom -= iBottomBarHeight;
  m_lcd.fillRect(rFill); //, ILI9341_OLIVE);
  m_lcd.DUMP();
}


void View::updateMaybe(unsigned long now)
{
  if(now < m_ulToUpdate)
    return;
  m_ulToUpdate = now + ulUpdatePeriod;
  update(now);
}

/** 
 *  m_position was just changed.  Default implementation updates m_rectClient
 */
void View::onPosition()
{
  m_rectClient = m_position;
  m_rectClient.top += iTitleBarHeight;
  m_rectClient.bottom -= iBottomBarHeight;
}


/** 
 * Entire screen redraw
 * Update non-clietn area and then call a virtual update of the client in a safe sandbox
 */
void View::update(unsigned long now)
{
  DUMP("View::update()");    

  drawTitleBar();
  drawSoftLabels(); 
  //
  // try to protect non-client area  
  // 
  m_lcd.setClipRect(m_rectClient);
  
  //
  // set defaults for use in the client area
  //
  m_lcd.setFont(LiberationSans_18);  
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
  DEBUG_PRINTLN("View::updateClient() SHOULD BE OVERWRITTEN");
  // entire background erase - does the job but blinks!
  m_lcd.fillRect(m_rectClient, ILI9341_BLUE);
}

/**
 *  draws the title bar
 */
void View::drawTitleBar()
{
  /*DEBUG_PRINT("View::drawTitleBar(");
  DEBUG_PRINT(m_szTitle);
  DEBUG_PRINTLN(") => ");*/
  //
  // draw the title itself
  //
  m_lcd.setFont(LiberationSans_18);
  //m_lcd.setTextSize(1);
  m_lcd.setTextColor(ILI9341_YELLOW,ILI9341_BLACK);
    
  int16_t w = m_lcd.measureTextWidth(m_szTitle);
  RECT r;
  r.top = m_position.top + 2;
  r.bottom = r.top + m_lcd.fontLineSpace();
  r.left = m_position.left + 2;
  r.right = r.left + w;
  
  m_lcd.setCursor(r.left, r.top);
  m_lcd.setClipRect(r);
  m_lcd.print(m_szTitle);
  m_lcd.ILI9341_t3::setClipRect();
  // clear the space between end of the title and the battery icon
  r.left = r.right;
  r.right = m_position.right - iBatteryWidth;
  if(r.left < r.right)
    m_lcd.fillRect(r, ILI9341_BLACK);

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

  //DEBUG_PRINT("Battery width: ");
  //DEBUG_PRINTDEC(m_lcd.measureTextWidth(szText));
  //DEBUG_PRINTLN("");  
  int16_t x = m_lcd.width() - m_lcd.measureTextWidth(szText) - 1;
  int16_t y = 0;
  
  m_lcd.setCursor(x, y);
  m_lcd.print(szText);
}

void View::drawButton(int16_t x, int16_t y, int16_t w, int16_t h, const ILI9341_t3_font_t *pFont, const char *szLabel)
{ 
  RECT rFill;
  m_lcd.drawRoundRect(x, y, w, h, iButtonCornerRadius, uButtonBorderColor);
  x += iButtonCornerRadius;
  int16_t y0 = y;
  y += iButtonCornerRadius;
  w -= 2*iButtonCornerRadius;
  int16_t h0 = h;
  h -= 2*iButtonCornerRadius;
  rFill.left = x;
  rFill.right = x+w;
  rFill.top = y;
  rFill.bottom = y+h;
  m_lcd.setClipRect(rFill);
  if(szLabel == 0)
  {
    m_lcd.fillRect(rFill, uButtonFaceColor); // clear the entire button face
  }
  else
  {
    m_lcd.setFont(*pFont);  
    int16_t tw = m_lcd.measureTextWidth(szLabel);
    int16_t th = m_lcd.measureTextHeight(szLabel);
    int16_t gw = (w-tw)/2;
    rFill.right = x+gw;
    m_lcd.fillRect(rFill, uButtonFaceColor); // clear the entire button face
    x += gw;
    m_lcd.setCursor(x, y0 + ((h0 - th)/2));
    m_lcd.print(szLabel);
    x += tw;
    rFill.left = x;
    rFill.right = x+gw;
    m_lcd.fillRect(rFill, uButtonFaceColor); // clear the entire button face
  }  
  m_lcd.ILI9341_t3::setClipRect();
}

/**
 * Draw bottom row with labels for soft keys
 * returns the bar height
 */
void View::drawSoftLabels()
{
  m_lcd.setTextColor(uButtonLabelColor, uButtonFaceColor);
  int16_t iButtonWidth = (m_lcd.width() / 3) - 12;
  int16_t iButtonHeight = iBottomBarHeight; // m_lcd.fontLineSpace() + 2*iButtonCornerRadius;
  int16_t y = m_lcd.height() - iButtonHeight;
  drawButton(0, y, iButtonWidth, iButtonHeight, m_fontSoftA, m_szSoftALabel);
  drawButton((m_lcd.width() - iButtonWidth)/2, y, iButtonWidth, iButtonHeight, m_fontNav, m_szNavLabel);
  drawButton(m_lcd.width() - iButtonWidth, y, iButtonWidth, iButtonHeight, m_fontSoftB, m_szSoftBLabel);
}

static const char szSeparator[] = ": ";

/**
 * Print Key1: Val1   Key2: Val2
 * with Key in ILI9341_DARKGREY
 * and Val in ILI9341_WHITE
 */
void View::printKeyVal(uint16_t y, const char *szKey1, long lVal1, const char *szKey2, long lVal2)
{
  char szText[80];
  uint16_t x = m_rectClient.width() / 4;             // position of the first ':'
  uint16_t w = m_lcd.measureTextWidth(szKey1);       // key width
  RECT rFill;
  rFill.top = rFill.bottom = y;
  rFill.bottom += m_lcd.fontLineSpace();
  rFill.left = m_rectClient.left;
  rFill.right = x - w;
  m_lcd.fillRect(rFill, ILI9341_BLACK);              // wipe space between rClient.left and key1
  m_lcd.setTextColor(ILI9341_DARKGREY, ILI9341_BLACK);
  m_lcd.setCursor(rFill.right, y);
  m_lcd.print(szKey1);                               // print key1
  m_lcd.setCursor(x, y);
  m_lcd.print(szSeparator);
  x += m_lcd.measureTextWidth(szSeparator);  
  m_lcd.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  m_lcd.setCursor(x, y);
  sprintf(szText, "%ld", lVal1);                     // print val1
  m_lcd.print(szText);
  x += m_lcd.measureTextWidth(szText);
  if(szKey2 != 0)
  {
    rFill.left = x;
    x = (m_lcd.width() / 4) * 3;                       // position of second ':'
    w = m_lcd.measureTextWidth(szKey2);
    rFill.right = x - w;
    m_lcd.fillRect(rFill, ILI9341_BLACK);              // wipe space between val1 and key2
    m_lcd.setTextColor(ILI9341_DARKGREY, ILI9341_BLACK);
    m_lcd.setCursor(rFill.right, y);
    m_lcd.print(szKey2);                               // print key2
    m_lcd.setCursor(x, y);
    m_lcd.print(szSeparator);
    x += m_lcd.measureTextWidth(szSeparator);  
    m_lcd.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    m_lcd.setCursor(x, y);
    sprintf(szText, "%ld", lVal2);
    m_lcd.print(szText);                               // print val2
    x += m_lcd.measureTextWidth(szText);
  }
  if(x < m_rectClient.right)
  {
    rFill.left = x;
    rFill.right = m_rectClient.right;
    m_lcd.fillRect(rFill, ILI9341_BLACK);
  }
}

/** dummy defaults, children to overwrite */
void View::onKeyDown(uint8_t vk) {
}
void View::onLongKeyDown(uint8_t vk) {
}
void View::onKeyUp(uint8_t vk) {
}

/** updateClient implementation for Run or Paused view */
void View::updateClientRunOrPaused(unsigned long now, bool bExtendedInfo, const char *pMsg)
{
  uint16_t y = m_rectClient.top;
  y += m_lcd.fontLineSpace(); 
  
  printKeyVal(y, "Pos", g_pPanner->currentPosition(), "Speed", (long)g_pPanner->speed());
  y += m_lcd.fontLineSpace();

  if(bExtendedInfo)
  {   
    y += 2;  
    const char *pLabel = 0;
    unsigned wSecs = g_ci.getBusySeconds(now);
    if(g_ci.isResting()) {
      pLabel = "Rest";
      unsigned long ulNext = g_ci.getNext();
      wSecs = (now < ulNext) ? ((ulNext - now) / 1000) : 0;
    } else if(g_ci.isWaitingForCompletion()) {
      pLabel = "Wait";
    } else if(g_ci.isPaused()) {
      pLabel = "Paused";
    } else {
      pLabel = "Stopped";
    }  
    printKeyVal(y, pLabel, wSecs);
    y += m_lcd.fontLineSpace();
  }
  if((pMsg != 0) && (pMsg[0] != '\0'))
  {
    y = m_rectClient.bottom - m_lcd.fontLineSpace();
    m_lcd.setTextColor(ILI9341_DARKGREY, ILI9341_BLACK);
    printTextLeft(pMsg, y, /*const ILI9341_t3_font_t *pFont =*/ 0);
    //y += m_lcd.fontLineSpace();
  } 
  //g_pPanner->DUMP();
}

#ifdef DEBUG
void View::DUMP(const char *szText /*= 0*/)
{
  Widget::DUMP(szText);
  DEBUG_PRINT("View@"); DEBUG_PRINTDEC((int)this); 
  DEBUG_PRINT(" m_szTitle="); DEBUG_PRINTLN(m_szTitle); 
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
  View(szTitle, &LiberationSans_18, szCancel, &LiberationSans_18, 0, &LiberationSans_18, szOK),
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
  /*DEBUG_PRINT("ModalDialog::drawTitleBar(");
  DEBUG_PRINT(m_szTitle);
  DEBUG_PRINTLN(") => ");*/
  
  m_lcd.drawLine(m_position.left, m_position.top, m_position.right, m_position.top, ILI9341_DARKGREY);
  //
  // draw the title itself
  //
  m_lcd.setFont(LiberationSans_18);
  //m_lcd.setTextSize(1);
  m_lcd.setTextColor(ILI9341_YELLOW,ILI9341_BLACK);
  
  int16_t w = m_lcd.measureTextWidth(m_szTitle);
  int16_t x = m_position.left + ((m_position.width()-w)/2);
  int16_t y = m_position.top + 3;
  
  RECT rFill;
  rFill.top = y;
  rFill.bottom = rFill.top + m_lcd.fontLineSpace();
  rFill.left = m_position.left;
  rFill.right = x;
  m_lcd.fillRect(rFill, ILI9341_BLACK); 
  m_lcd.setCursor(x, y);
  m_lcd.print(m_szTitle);
  rFill.left = x + w;
  rFill.right = m_position.right;
  m_lcd.fillRect(rFill, ILI9341_BLACK);
}

void ModalDialog::onKeyUp(uint8_t vk)
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
      return;
  }  
  // Close thge dialog and activate parent
  activate(m_zParent);
}

/**
 *  Position the modal dialog so that it does NOT obscure its parent
 */
void ModalDialog::onActivate(View *pPrevActive)
{
  m_iRes = IDUNKNOWN;
  m_zParent = pPrevActive;
  if(pPrevActive != 0)
    setPosition(pPrevActive->m_position.left, pPrevActive->m_position.top + iTitleBarHeight, pPrevActive->m_position.right, pPrevActive->m_position.bottom);
  // erase the entire background
  RECT rFill = m_position;
  rFill.bottom -= iBottomBarHeight;
  m_lcd.fillRect(rFill, ILI9341_DARKCYAN); // ILI9341_BLACK
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
  printTextCenter(m_strMessage.c_str(), m_rectClient.top + (m_rectClient.height()/2));
}

