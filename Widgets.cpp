#include "Panner.h"
#include "BatteryMonitor.h"
#include <font_LiberationSans.h>
#include <font_AwesomeF000.h>
#include <font_AwesomeF100.h>
#include <font_AwesomeF200.h>

/**
 *  RECT Class Implementation
 */
#ifdef DEBUG
void RECT::DUMP(const char *szText /* = 0*/) 
{
  if(szText != 0) {
    DEBUG_PRINT(szText);
  }
  DEBUG_PRINT(" RECT@"); DEBUG_PRINTDEC((int)this); 
  DEBUG_PRINT(" left="); DEBUG_PRINTDEC((int)left);
  DEBUG_PRINT(" top="); DEBUG_PRINTDEC((int)top);
  DEBUG_PRINT(" right="); DEBUG_PRINTDEC((int)right);
  DEBUG_PRINT(" bottom="); DEBUG_PRINTDEC((int)bottom);
  DEBUG_PRINTLN("");  
}
#endif
/**
 *  Widget Class Implementation
 */
void Widget::draw() 
{
  // just to make sure we overwrite this....
  m_lcd.fillRect(m_position, ILI9341_RED);
  m_lcd.fillRect(m_rectClient, ILI9341_CYAN);
}

void Widget::setPosition(int16_t left, int16_t top, int16_t right, int16_t bottom) 
{
  m_position.left = left;
  m_position.top = top;
  m_position.right = right;
  m_position.bottom = bottom;
  onPosition();
}

#ifdef DEBUG
void Widget::DUMP(const char *szText /*= 0*/)
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
 
/**
 *  print text left-aligned in the client area
 */
void Widget::printTextLeft(const char *szText, uint16_t y, const ILI9341_t3_font_t *pFont/* = 0 */)
{
  //DEBUG_PRINT("Widget::printTextLeft(");DEBUG_PRINT(szText);DEBUG_PRINTLN(")");
  //DUMP("Widget::printTextLeft");
  
  const ILI9341_t3_font_t *pOldFont = 0;
  if(pFont != 0) {
    pOldFont = m_lcd.getFont();
    m_lcd.setFont(*pFont);
  }
  int16_t x = m_rectClient.left;
  //DEBUG_PRINT("x=");DEBUG_PRINTDEC(x);DEBUG_PRINTLN("");
  m_lcd.setCursor(x, y);
  m_lcd.print(szText);
  x += m_lcd.measureTextWidth(szText);
  //DEBUG_PRINT("x=");DEBUG_PRINTDEC(x);DEBUG_PRINTLN("");
  if(x < m_rectClient.right)
  {
    RECT rFill;
    rFill.top = y;
    rFill.bottom = y + m_lcd.fontLineSpace();
    rFill.left = x;
    rFill.right = m_rectClient.right;
    m_lcd.fillRect(rFill, ILI9341_BLACK);
  }
  if(pOldFont != 0)
    m_lcd.setFont(*pOldFont);  
}
/**
 *  print text centered in the client area using current font
 */
void Widget::printTextCenter(const char *szText, uint16_t y, const ILI9341_t3_font_t *pFont /* = 0*/, int16_t *pDY /*= 0*/)
{
  const ILI9341_t3_font_t *pOldFont = 0;
  if(pFont != 0) {
    pOldFont = m_lcd.getFont();
    m_lcd.setFont(*pFont);
  }
  RECT rFill;
  rFill.left = m_rectClient.left;
  rFill.top = y;
  rFill.bottom = y + m_lcd.fontLineSpace();
  int16_t tw = m_lcd.measureTextWidth(szText);
  int16_t x1 = m_rectClient.left + (m_rectClient.width() - tw)/2;
  rFill.right = x1;  
  m_lcd.fillRect(rFill, ILI9341_BLACK);
  m_lcd.setCursor(x1, y);
  m_lcd.print(szText);
  x1 += tw;
  if(x1 < m_rectClient.right)
  {
    rFill.left = x1;
    rFill.right = m_rectClient.right;
    m_lcd.fillRect(rFill, ILI9341_BLACK);
  }
  if(pDY != 0)
    *pDY = rFill.height();    
  if(pOldFont != 0)
    m_lcd.setFont(*pOldFont);  
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
  char szText[80];
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

 
/**
 *  TextWidget Class Implementation
 */
void TextWidget::draw()
{
  DEBUG_PRINT("TextWidget::draw: "); DEBUG_PRINTLN(m_strText.c_str());
  m_position.DUMP("TextWidget m_position: ");
  m_rectClient.DUMP("TextWidget m_rectClient: ");

  printTextCenter(m_strText.c_str(), 0, m_pFont, 0);
}

/**
 *  ListWidget Class Implementation
 */
ListWidget::ListWidget(uint16_t selectionMode, const ILI9341_t3_font_t *pFont /*= 0*/) :  
  Widget(pFont), 
  m_selectionMode(selectionMode) 
{
  hasBorder(true);
}
 
/** 
 *  m_position was just changed.  Default implementation updates m_rectClient
 */
void ListWidget::onPosition()
{
  DEBUG_PRINTLN("ListWidget::onPosition()");
  m_rectClient = m_position;
  if(hasBorder())
    m_rectClient.deflate();
}

/**
 * Display the list of strings
 */
void ListWidget::draw()
{
  //DEBUG_PRINTLN("ListWidget::draw()");
  //m_position.DUMP("ListWidget m_position: ");
  //m_rectClient.DUMP("ListWidget m_rectClient: ");
  if(hasBorder())
    m_lcd.drawRect(m_position.left, m_position.top, m_position.width(), m_position.height(), ILI9341_DARKGREY);
  
  RECT rectOldClip;
  m_lcd.getClipRect(rectOldClip);
  m_lcd.setClipRect(m_rectClient);

  //int16_t x = m_rectClient.left;
  int16_t y = m_rectClient.top;
  int16_t iSel = getCurSel();
  m_iFirstDisplayed = sanitize((m_selectionMode != smNoSelection) ? (iSel - 1) : m_iFirstDisplayed);
  if(m_iFirstDisplayed > 0)
  {
    int16_t dY = 0;
    printTextCenter("\x06", y, &AwesomeF100_14, &dY);
    y += dY;
  }
  //
  if(m_pFont != 0)
    m_lcd.setFont(*m_pFont);
  int16_t iFontLineSpace = m_lcd.fontLineSpace();
  for(size_t i = m_iFirstDisplayed; i < m_items.size(); i++)
  {
    if(y + iFontLineSpace > m_rectClient.bottom)
    {
      int16_t dY = 0;
      printTextCenter("\x07", y, &AwesomeF100_14, &dY);
      y += dY;
      break;
    }
    drawItem(i, y);
    y += iFontLineSpace;
  }
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

void ListWidget::drawItem(size_t i, int16_t y)
{
  printTextLeft(m_items[i].c_str(), y);
  if(i == (size_t)getCurSel())
    m_lcd.drawRect(m_rectClient.left, y, m_rectClient.width(), m_lcd.fontLineSpace(), ILI9341_DARKGREY);
}

int16_t ListWidget::sanitize(int16_t iSel)
{
  if(iSel >=  (int16_t)m_items.size())
    return m_items.size() - 1;
  if(iSel < 0)
    return 0;
  return iSel;  
}

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

/**
 *  Small drop down list.  
 *  When not dropped shows just a single item with an arrow down
 */

/** */
/*void DropDownListWidget::draw()
{
  
}*/

/** 
 * In the closed state the height is determined by the Widget font and can not be changed.
 * RECT m_position and RECT m_rectClient are for closed state only;
 */
/*void DropDownListWidget::onPosition()
{
  
}*/



/** 
 * KeyValueListWidget Class Implementation
 */

/** 
 * reset content 
 */
void KeyValueListWidget::clear() 
{
  m_items.clear();
  m_values.clear();
}

/** 
 * add a Key/Value pair to the KeyValueListWidget 
 */
void KeyValueListWidget::push_back(std::string key, long val)
{
  m_items.push_back(key);    
  m_values[key] = val;
}

/** 
 * get the value of the currently selected key 
 */
long KeyValueListWidget::getCurValue() 
{
  if((m_iCurSel < 0) || (m_iCurSel >= (int16_t)m_items.size()))
    return LB_ERR;
  std::string key = m_items[m_iCurSel];
  if(m_values.count(key) == 0)
    return LB_ERR;
  return m_values[key];
}

/** 
 * set the value of the currently selected key.  Return LB_ERR in case of error 
 */
int16_t KeyValueListWidget::setCurValue(long lVal)
{
  if((m_iCurSel < 0) || (m_iCurSel >= (int16_t)m_items.size()))
    return LB_ERR;
  std::string key = m_items[m_iCurSel];
  if(m_values.count(key) == 0)
    return LB_ERR;
  m_values[key] = lVal;
  return 0;
}

void KeyValueListWidget::KeyValueListWidget::drawItem(size_t i, int16_t y)
{
  i = (size_t)sanitize((int16_t)i);
  std::string key = m_items[i];
  if(m_values.count(key) == 0)
    ListWidget::drawItem(i, y);
  else
    drawItem(key, m_values[key], y, (i == (size_t)getCurSel()));
}

void KeyValueListWidget::drawItem(std::string &key, long lVal, int16_t y, bool bSelected)
{
  uint16_t x = (2 * m_rectClient.width()) / 3;             // position of the ':'
  printKeyVal(x, y, key.c_str(), lVal, bSelected);
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
    std::string key = m_items[iSel];
    if(m_values.count(key) == 0)
      continue;
    setCurSel(iSel);
    return 0;
  }
  return LB_ERR;
}

