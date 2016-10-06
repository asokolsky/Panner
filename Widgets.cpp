#include "Panner.h"
#include "BatteryMonitor.h"
#include <font_LiberationSans.h>
//#include <font_AwesomeF000.h>
#include <font_AwesomeF080.h>
//#include <font_AwesomeF100.h>
//#include <font_AwesomeF200.h>


/**
 *  Widget Class Implementation
 */
void Widget::draw() 
{
  // just to make sure we overwrite this....
  m_lcd.fillRect(m_position, ILI9341_RED);
  m_lcd.fillRect(m_rectClient, ILI9341_CYAN);
}

void Widget::setPosition(const RECT rLoc)
{
  //if((left == m_position.left) && (m_position.top == top) && (m_position.right == right) && (m_position.bottom == bottom)) return;
  m_position = rLoc;
  onPosition();
}

#ifdef DEBUG
void Widget::DUMP(const char *szText /*= 0*/) const
{
  if(szText != 0) {
    DEBUG_PRINT(szText);
  }
  DEBUG_PRINT(" Widget@"); DEBUG_PRINTDEC((int)this); 
  m_position.DUMP(" m_position: ");
  m_rectClient.DUMP("m_rectClient: ");  
}
#endif

/** 
 *  m_position was just changed.  
 *  Default implementation updates m_rectClient leaving no space for decorations.
 */
void Widget::onPosition()
{
  m_rectClient = m_position;
}
 
static const char szSeparator[] = ": ";

/**
 * Print Key1: Val1   Key2: Val2
 * with Key in ILI9341_DARKGREY
 * and Val in ILI9341_WHITE
 * x is the position of the first ':'
 */
void Widget::printKeyVal(uint16_t x, uint16_t y, const char *szKey1, long lVal1, bool bSelected, const char *szKey2, long lVal2)
{
  RECT rLocation;
  rLocation.left = m_rectClient.left;
  rLocation.right = x;
  rLocation.top = y;
  rLocation.bottom = 0;
  //rLocation.DUMP("Widget::printKeyVal rLocation");
  
  m_lcd.printText(szKey1, ILI9341_DARKGREY, ILI9341_BLACK, rLocation, Display::haRight, Display::vaTop);
  m_lcd.setCursor(x, y);
  m_lcd.print(szSeparator);
  x += m_lcd.measureTextWidth(szSeparator);
  
  m_lcd.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  m_lcd.setCursor(x, y);
  char szText[80];
  sprintf(szText, "%ld", lVal1);                     // print val1
  m_lcd.print(szText);
  if(bSelected) {
    RECT r;
    r.top = y-1;
    r.bottom = r.top + m_lcd.fontLineSpace() + 1;
    r.left = x-1;
    r.right = r.left + m_lcd.measureTextWidth(szText) + 3;
    m_lcd.drawRect(r.left, r.top, r.width(), r.height(), ILI9341_DARKGREY);
    x += 3;
  }  
  x += m_lcd.measureTextWidth(szText);
  if(szKey2 != 0)
  {
    rLocation.left = x;
    x = rLocation.right = (m_rectClient.width() / 4) * 3;         // position of second ':'
    //rLocation.top = y;
    //rLocation.bottom = 0;
    m_lcd.printText(szKey2, ILI9341_DARKGREY, ILI9341_BLACK, rLocation, Display::haRight, Display::vaTop);
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
    rLocation.left = x;
    rLocation.right = m_rectClient.right;
    m_lcd.fillRect(rLocation, ILI9341_BLACK);
  }
}

 
/**
 *  TextWidget Class Implementation
 */
/*void TextWidget::draw()
{
  DEBUG_PRINT("TextWidget::draw: "); DEBUG_PRINTLN(m_strText.c_str());
  m_position.DUMP("TextWidget m_position: ");
  m_rectClient.DUMP("TextWidget m_rectClient: ");

  printTextCenter(m_strText.c_str(), 0, m_pFont, 0);
}*/

/**
 *  ListWidget Class Implementation
 */
ListWidget::ListWidget(uint16_t selectionMode, const ILI9341_t3_font_t *pFont /*= 0*/) :  
  Widget(pFont), 
  m_selectionMode(selectionMode) 
{
  hasBorder(true);
}

void ListWidget::push_back(const char str[])
{
  m_items.push_back(str);
}
 
/** 
 *  m_position was just changed.  Default implementation updates m_rectClient
 */
void ListWidget::onPosition()
{
  Widget::onPosition(); // m_rectClient = m_position;
  if(hasBorder())
    m_rectClient.deflate();
  //DUMP("ListWidget::onPosition()");
}

/**
 * Display the list of strings
 */
void ListWidget::draw()
{
  //DUMP("ListWidget::draw()");
  if(hasBorder())
    m_lcd.drawRect(m_position.left, m_position.top, m_position.width(), m_position.height(), ILI9341_DARKGREY);
  
  RECT rectOldClip = m_lcd.getClipRect();  
  //rectOldClip.DUMP("rectOldClip");  
  RECT rClip = rectOldClip.intersect(m_rectClient);
  m_lcd.setClipRect(rClip);
  //rClip.DUMP("rClip");

  //
  if(m_pFont != 0)
    m_lcd.setFont(*m_pFont);
  int16_t iFontLineSpace = m_lcd.fontLineSpace();
  int16_t yBot = m_rectClient.bottom - max(iFontLineSpace, AwesomeF080_14.line_space);
  //DEBUG_PRINT("yBot="); DEBUG_PRINTDEC(yBot); DEBUG_PRINTLN();
  //DEBUG_PRINT("iFontLineSpace="); DEBUG_PRINTDEC(iFontLineSpace); DEBUG_PRINTLN();
  //DEBUG_PRINT("AwesomeF080_14.line_space="); DEBUG_PRINTDEC(AwesomeF080_14.line_space); DEBUG_PRINTLN();

  RECT rLoc = m_rectClient;
  int16_t y = m_rectClient.top;
  //DEBUG_PRINT("y="); DEBUG_PRINTDEC(y); DEBUG_PRINTLN();  
  if(m_selectionMode != smNoSelection)
  {
    int16_t iItemsDisplayed = (m_rectClient.height() / (iFontLineSpace + 2)) -2;
    if(iItemsDisplayed <= m_iCurSel)
      m_iFirstDisplayed = 0;   
    m_iFirstDisplayed = sanitize(m_iCurSel - iItemsDisplayed);
  }
  if(m_iFirstDisplayed > 0)
  {
    y += AwesomeF080_14.line_space;
    rLoc.bottom = y;
    m_lcd.printText("\x58", m_lcd.getTextColor(), ILI9341_BLACK, rLoc, Display::haCenter, Display::vaCenter, &AwesomeF080_14, false);  /* /\ */
  }
  
  for(size_t i = m_iFirstDisplayed; i < m_items.size(); i++)
  {
    if(y >= yBot)
    {
      rLoc.top = y;
      y += AwesomeF080_14.line_space;
      rLoc.bottom = y;
      m_lcd.printText("\x57", m_lcd.getTextColor(), ILI9341_BLACK, rLoc, Display::haCenter, Display::vaCenter, &AwesomeF080_14, false);  /* \/ */
      break;
    }
    //y++;
    rLoc.top = y;
    y += iFontLineSpace + 2;
    rLoc.bottom = y;
    drawItem(i, rLoc);
    
    //DEBUG_PRINT("y="); DEBUG_PRINTDEC(y); DEBUG_PRINTLN();
  }
  //DEBUG_PRINT("Finally: y="); DEBUG_PRINTDEC(y); DEBUG_PRINTLN();
  if(y < m_rectClient.bottom)
  {
    RECT rFill;
    rFill.left = m_rectClient.left;
    rFill.right = m_rectClient.right;
    rFill.top = y;
    rFill.bottom = m_rectClient.bottom;
    m_lcd.fillRect(rFill, ILI9341_BLACK);
  }
  m_lcd.setClipRect(rectOldClip);
}

void ListWidget::drawItem(size_t iItem, RECT rLoc)
{
  m_lcd.printText(m_items[iItem].c_str(), m_lcd.getTextColor(), ILI9341_BLACK, rLoc, Display::haLeft, Display::vaCenter, 0, false);
  if(iItem == (size_t)m_iCurSel)
    m_lcd.drawRect(rLoc.left, rLoc.top, rLoc.width(), rLoc.height(), ILI9341_DARKGREY);
}

int16_t ListWidget::sanitize(int16_t iSel)
{
  if(iSel >=  (int16_t)m_items.size())
    return m_items.size() - 1;
  if(iSel < 0)
    return 0;
  return iSel;  
}

/**
 * AdvanceSelection in the list of text items - simple!
 */
int16_t ListWidget::advanceSelection(int16_t iAdv /*= 1*/)
{
  if(m_selectionMode != smSingleSelection)
    return LB_ERR;
  setCurSel(sanitize(getCurSel() + iAdv));
  return 0;
}

void ListWidget::scroll(int16_t iAdv /*= 1*/)
{
  m_iFirstDisplayed = sanitize(m_iFirstDisplayed + iAdv);
}

#ifdef DEBUG
void ListWidget::DUMP(const char *szText /*= 0*/)
{
  if(szText != 0) {
    DEBUG_PRINT(szText);
  }
  DEBUG_PRINT(" ListWidget@"); DEBUG_PRINTDEC((int)this); DEBUG_PRINT(" m_selectionMode="); DEBUG_PRINTDEC((int)m_selectionMode); 
  DEBUG_PRINT(" m_iCurSel="); DEBUG_PRINTDEC((int)m_iCurSel);  DEBUG_PRINT(" m_iFirstDisplayed="); DEBUG_PRINTDEC((int)m_iFirstDisplayed); 
  
  m_position.DUMP(" m_position: ");
  m_rectClient.DUMP("m_rectClient: ");  
  
}
#endif

/**
 *  ListSpinnerWidget
 */

/**
 * AdvanceSelection in the list of text strings or number spinner!
 */
int16_t ListSpinnerWidget::advanceSelection(int16_t iAdv /*= 1*/)
{
  if(m_selectionMode == smMultiSelection)
  {
    // this is a number spinner!!
    m_iCurSel += iAdv;  // uninhibited advance!
    return 0;
  }
  if(m_selectionMode == smNoSelection)
  {
    // this is a null spinner!!
    return LB_ERR;
  }
  return ListWidget::advanceSelection(iAdv);
}

/** 
 *  Draw any of ListSpinnerWidget or NumberSpinnerWidget or NullSpinnerWidget
 */
void ListSpinnerWidget::draw()
{
  //DUMP("ListSpinnerWidget::draw()");
  //
  // first - non-client area
  //
  const char *szArrow = 0; // nothing by default
  if(hasFocus())
  {
    // draw up/down arrows! if we have focus only.
    szArrow = "\x5C"; // up/down
    if(m_selectionMode == smSingleSelection)
    {
      if(m_iCurSel == 0)
        szArrow =  "\x5E"; // down only
      else if((m_iCurSel + 1) == (int16_t)m_items.size())
        szArrow = "\x5D";  // up only
    }
  }
  RECT rLocation = m_position;
  rLocation.left = m_rectClient.right;
  //m_lcd.drawRect(rLocation.left, rLocation.top, rLocation.width(), rLocation.height(), ILI9341_WHITE);
  m_lcd.printText(szArrow, ILI9341_WHITE, ILI9341_BLACK, rLocation, Display::haLeft, Display::vaCenter, &AwesomeF080_12);
  if(hasBorder())
    m_lcd.drawRect(m_position.left, m_position.top, m_position.width(), m_position.height(), ILI9341_DARKGREY);
  //
  // now client area
  //  
  RECT rectOldClip = m_lcd.getClipRect();  
  RECT rClip = rectOldClip.intersect(m_rectClient);
  m_lcd.setClipRect(rClip);  
  if(m_pFont != 0)
    m_lcd.setFont(*m_pFont);
  if(m_selectionMode == smMultiSelection)
  {
    // this is a number spinner!!
    char szText[80];
    sprintf(szText, "%d", m_iCurSel);
    m_lcd.printText(szText, ILI9341_WHITE, ILI9341_BLACK, m_rectClient, Display::haLeft, Display::vaCenter);
  }
  else if (m_selectionMode == smNoSelection)
  {
    // this is a null spinner!!
    ;
  }
  else
  {
    // text list spinner
    m_lcd.printText(
      ((0 <= m_iCurSel) && (m_iCurSel < (int16_t)m_items.size())) ? m_items[m_iCurSel].c_str() : 0, 
      ILI9341_WHITE, ILI9341_BLACK, m_rectClient, Display::haLeft, Display::vaCenter);
  }
    
  m_lcd.setClipRect(rectOldClip);
}

/** 
 * The height is determined by the Widget font and can not be changed.
 */
void ListSpinnerWidget::onPosition()
{
  // adjust m_position to just one line of text high
  int16_t h = (m_pFont != 0) ? m_pFont->line_space : m_lcd.fontLineSpace();
  if(hasBorder())
    h += 2;
  int16_t bot = m_position.top + h;
  if(bot < m_position.bottom)
    m_position.bottom = bot;
  // do the default behaviour - m_rectClient = m_position;
  ListWidget::onPosition();
  m_rectClient.right -= 16; // space for the spinner arrow(s)
  if(m_rectClient.right < m_rectClient.left)
    m_rectClient.right = m_rectClient.left;
  //DUMP("ListSpinnerWidget::onPosition()");
}


/** 
 * reset content 
 */
void KeyValueListWidget::clear() 
{
  m_items.clear();
  m_values.clear();
}

/** 
 * add a Key to the KeyValueListWidget 
 */
void KeyValueListWidget::push_back(const char key[])
{
  m_items.push_back(key);
  NullSpinnerWidget w;
  m_values.push_back(w);
}

/** 
 * add a Key/NumValue pair to the KeyValueListWidget 
 */
void KeyValueListWidget::push_back(const char key[], int16_t val)
{
  m_items.push_back(key);
  NumberSpinnerWidget w(val);
  m_values.push_back(w);
}

/** 
 * add a Key/ListValue pair to the KeyValueListWidget 
 */
void KeyValueListWidget::push_back(const char key[], ListSpinnerWidget &val)
{
  m_items.push_back(key);
  m_values.push_back(val);
}

void KeyValueListWidget::drawItem(size_t i, RECT rLoc)
{
  i = (size_t)sanitize((int16_t)i);
  if(m_values[i].getSelectionMode() == smNoSelection)
  {
    // the only reason NullSpinnerWidget exists
    ListWidget::drawItem(i, rLoc);
    return;
  }
  uint16_t x = (2 * m_rectClient.width()) / 3;             // position of the ':'

  RECT rLocation = rLoc;
  rLocation.right = x;
  m_lcd.printText(m_items[i].c_str(), ILI9341_DARKGREY, ILI9341_BLACK, rLocation, Display::haRight, Display::vaCenter);

  rLocation.left = rLocation.right;
  rLocation.right += m_lcd.measureTextWidth(szSeparator);
  m_lcd.printText(szSeparator, ILI9341_DARKGREY, ILI9341_BLACK, rLocation, Display::haLeft, Display::vaCenter); 
  rLocation.left = rLocation.right;
  rLocation.right = m_rectClient.right;
  m_values[i].setPosition(rLocation);
  m_values[i].hasFocus(i == (size_t)getCurSel());
  m_values[i].draw();
}

/**
 * advance selection ignoring non-adjustable items 
 */
int16_t KeyValueListWidget::advanceSelection(int16_t iAdv /*= 1*/)
{
  if(m_selectionMode != smSingleSelection)
    return LB_ERR;
  for(int16_t iSel = m_iCurSel + iAdv; (0 <= iSel) && (iSel < (int16_t)m_items.size()) ; (iAdv > 0) ? iSel++ : iSel--)
  {
    //if(m_values[iSel].getSelectionMode() == smNoSelection) continue;
    setCurSel(iSel);
    return 0;
  }
  return LB_ERR;
}

/** 
 * get the value of the currently selected key 
 */
ListSpinnerWidget *KeyValueListWidget::getCurValue()
{
  if((m_iCurSel < 0) || (m_iCurSel >= (int16_t)m_values.size()))
    return 0;
  return &m_values[m_iCurSel];
}

/** 
 * get the value of the given key
 */
ListSpinnerWidget *KeyValueListWidget::getValue(const char key[])
{
  for(size_t i = 0; i < m_items.size(); i++)  
    if(m_items[i].compare(key) == 0)
      return &m_values[i];
  return 0;
}
/** 
 * get the value of the given key
 */
int16_t KeyValueListWidget::getNumericValue(const char key[])
{
  ListSpinnerWidget *p = getValue(key);
  if((p == 0) || (p->getSelectionMode() != smMultiSelection))
    return LB_ERR;
  return p->getCurSel();
}

/** 
 * get the value by index
 */
int16_t KeyValueListWidget::getNumericValue(int16_t i)
{
  if((i < 0) || (i >= (int16_t)m_values.size()))
    return LB_ERR;
  ListSpinnerWidget *p = &m_values[i];
  if(p->getSelectionMode() != smMultiSelection)
    return LB_ERR;
  return p->getCurSel();
}

