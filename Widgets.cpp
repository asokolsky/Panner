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
    printTextLeft(m_items[i].c_str(), y);
    if(i == (size_t)iSel) {
      m_lcd.drawRect(m_rectClient.left, y, m_rectClient.width(), iFontLineSpace, ILI9341_DARKGREY);
    }
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

int16_t ListWidget::sanitize(int16_t iSel)
{
  if(iSel >=  (int16_t)m_items.size())
    return m_items.size() - 1;
  if(iSel < 0)
    return 0;
  return iSel;  
}

void ListWidget::advanceSelection(int16_t iAdv /*= 1*/)
{
  setCurSel(sanitize(getCurSel() + iAdv));
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
void DropDownListWidget::draw()
{
  
}

/** 
 * In the closed state the height is determined by the Widget font and can not be changed.
 * RECT m_position and RECT m_rectClient are for closed state only;
 */
void DropDownListWidget::onPosition()
{
  
}

