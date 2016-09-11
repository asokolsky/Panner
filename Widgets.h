
class View;

/**
 *  Nano-windowing system for Arduino and derivatives
 */

const uint16_t WS_VISIBLE   =0x0001;
const uint16_t WS_CHILD     =0x0002;
const uint16_t WS_BORDER    =0x0004;
const uint16_t WS_TITLEBAR  =0x0008;
const uint16_t WS_HSCROLL   =0x0010;
const uint16_t WS_VSCROLL   =0x0020;
const uint16_t WS_LEFTTEXT  =0x0100;
const uint16_t WS_CENTERTEXT=0x0200;
 /**
  *  Root object that can be displayed on the screen
  */
class Widget
{
protected:
  uint16_t m_uStyle;

public:
  /** this is position of the widget */
  RECT m_position;
  /** this is client area - not including title, border, etc */
  RECT m_rectClient;
  /** Text font - if any  */
  const ILI9341_t3_font_t *m_pFont = 0;
  
  Widget(const ILI9341_t3_font_t *pFont = 0, uint16_t uStyle = 0) : m_pFont(pFont) {}

  bool hasBorder() {
    return (m_uStyle & WS_BORDER) != 0;
  }
  void hasBorder(bool bHasBorder) {
    if(bHasBorder)
      m_uStyle |= WS_BORDER;
    else
      m_uStyle &= ~WS_BORDER;
  }

  virtual void draw();
  /** 
   *  m_position was just changed.  Default implementation updates m_rectClient
   */
  virtual void onPosition();

  /**
   *  print text left-aligned in the m_position using current font
   */
  void printTextLeft(const char *szText, uint16_t y, const ILI9341_t3_font_t *pFont = 0);
  /**
   *  print text centered in the m_position using current font
   */
  void printTextCenter(const char *szText, uint16_t y, const ILI9341_t3_font_t *pFont = 0, int16_t *pDY = 0);
  /**
   *  Print Key: Val with ':' being at x
   */
  void printKeyVal(uint16_t x, uint16_t y, const char *szKey1, long lVal1, bool bSelected = false, const char *szKey2 = 0, long lVal2 = 0);
  

  void setPosition(int16_t left, int16_t top, int16_t right, int16_t bottom);
  void setPosition(RECT &r) {
    setPosition(r.left, r.top, r.right, r.bottom);
  }

  void setFont(const ILI9341_t3_font_t *pFont = 0) {
    m_pFont = pFont; 
  }
  const ILI9341_t3_font_t *getFont() {
    return m_pFont;
  }
  unsigned char getLineSpace() {
    return (m_pFont != 0) ? m_pFont->line_space : 0;
  }

#ifdef DEBUG
  void DUMP(const char *szText = 0);
#else
  void DUMP(const char *szText = 0) {}
#endif

};

/**
 * Static Text Widget with no scroll
 */
class TextWidget : public Widget
{
  std::string m_strText;
  
public:

  TextWidget(const ILI9341_t3_font_t *pFont = 0) : Widget(pFont) {}

  void setText(const char *msg) {
    m_strText = msg; 
  }
  
  /** Text is painted centered */
  void draw();
};

const int16_t LB_ERR = -1;

/** SelectionMode  */
const uint16_t smNoSelection = 0;
const uint16_t smSingleSelection = 1;
const uint16_t smMultiSelection = 2;

/**
 * List of strings with up/down scroll and border by default
 */
class ListWidget : public Widget
{
protected:  
  /** one of smXXX */
  uint16_t m_selectionMode; 
  /** no selection by default */
  int16_t m_iCurSel = LB_ERR;
  int16_t m_iFirstDisplayed = 0;

  int16_t sanitize(int16_t iSel);
  
public:
  std::vector<std::string> m_items;

  ListWidget(uint16_t selectionMode, const ILI9341_t3_font_t *pFont = 0);

  void clear() {
    m_items.clear();
  }

  int16_t getCurSel() {
    return m_iCurSel;
  }
  void setCurSel(int16_t iSel) {
    if((iSel < 0) || (iSel >= (int16_t)m_items.size()))
      iSel = LB_ERR;
    m_iCurSel = iSel;
  }
  /** 
   * makes sense only if m_selectionMode is smSingleSelection 
   */
  int16_t advanceSelection(int16_t iAdv = 1);
  /** makes sense only if m_selectionMode is smNoSelection */
  void scroll(int16_t iAdv = 1);
  /** display/paint the list */
  void draw();
  /** called from draw to draw a single item */
  virtual void drawItem(size_t i, int16_t y);
  /** recalc the client rect */
  void onPosition();
};

/**
 *  Small drop down list.  
 *  When not dropped shows just a single item with an arrow down
 */
/*class DropDownListWidget : public ListWidget
{
  bool m_bDropped = false;
  // RECT m_position and RECT m_rectClient are for closed state only;
  RECT m_positionDropped;
  RECT m_rectClientDropped;
  
public:  
  DropDownListWidget(const ILI9341_t3_font_t *pFont = 0) : ListWidget(smSingleSelection, pFont) {}

  void dropDown() {
    m_bDropped = true;
  }
  void dropClose() {
    m_bDropped = true;    
  } */
  
  /** */
  //void draw();
  /** the height is determined by the Widget font and can not be changed */
  //void onPosition();
//};

/**
 *  List of any one of:
 *    null for empty space
 *    string for plain text
 *    pair of key (string) value (number or enum - one of)
 */
class KeyValueListWidget : public ListWidget
{
public:  
  // std::vector<std::string> m_items; - this holds keys
  /** values associated with keys from m_items */
  std::map<std::string, long> m_values;

  KeyValueListWidget(const ILI9341_t3_font_t *pFont = 0) : ListWidget(smSingleSelection, pFont) {}

  /** reset content */
  void clear();
  /** add a Key/Value pair to the KeyValueListWidget */
  void push_back(std::string key, long val);
  /** get the value of the currently selected key */
  long getCurValue();
  /** 
   * set the value of the currently selected key.  
   * Return LB_ERR in case of error 
   */
  int16_t setCurValue(long lVal); 
  /** just show a single item */
  void drawItem(size_t i, int16_t y); 
  /** most work done here */
  void drawItem(std::string &key, long lVal, int16_t y, bool bSelected);

  /** 
   * advance selection ignoring non-adjustable items 
   * Return LB_ERR in case of error 
   */
  int16_t advanceSelection(int16_t iAdv = 1);
};

