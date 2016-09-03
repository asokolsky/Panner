/**
 *  Nano-windowing system for Arduino and derivatives
 */

 /**
  *  Root object that can be displayed on the screen
  */
class Widget
{
public:
  /** this is position of the widget */
  RECT m_position;
  /** this is client area - not including title, border, etc */
  RECT m_rectClient;
  
  Widget() {}

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

  void setPosition(int16_t left, int16_t top, int16_t right, int16_t bottom);
  void setPosition(RECT &r) {
    setPosition(r.left, r.top, r.right, r.bottom);
  }

  void DUMP(const char *szText = 0);
};

/**
 * Static Text Widget with no scroll
 */
class TextWidget : public Widget
{
  std::string m_strText;
  const ILI9341_t3_font_t *m_pFont;
  
public:

  TextWidget(const ILI9341_t3_font_t *pFont) : m_pFont(pFont) {}

  void setText(const char *msg) {
    m_strText = msg; 
  }
  void setFont(const ILI9341_t3_font_t *pFont = 0) {
    m_pFont = pFont; 
  }
  const ILI9341_t3_font_t *getFont() {
    return m_pFont;
  }
  unsigned char getLineSpace() {
    return m_pFont->line_space;
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
 * ListBox with a vertical scroll
 */
class ListWidget : public Widget
{
  /** one of smXXX */
  uint16_t m_selectionMode; 
  /** no selection by default */
  int16_t m_iCurSel = LB_ERR;
  
public:
  std::vector<std::string> m_items;

  ListWidget(uint16_t selectionMode) :  m_selectionMode(selectionMode) {}

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

  void advanceSelection(int16_t iAdv = 1);
  
  void draw();
  /** recalc the client rect */
  void onPosition();
};

/**
 *  Small drop down list.  
 *  When not dropped shows just a single item with an arrow down
 */
class DropDownListWidget : public ListWidget
{
  bool m_bDropped = false;
  // RECT m_position and RECT m_rectClient are for closed state only;
  RECT m_positionDropped;
  RECT m_rectClientDropped;
  
public:  
  DropDownListWidget() : ListWidget(smSingleSelection) {}

  void dropDown() {
    m_bDropped = true;
  }
  void dropClose() {
    m_bDropped = true;    
  }  
  
  /** */
  void draw();
  /** the height is determined by the Widget font and can not be changed */
  void onPosition();
};

