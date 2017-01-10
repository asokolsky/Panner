
/**
 *  Basic definition, derived from ILI9341_t3 or rather the fork of it by blackketter
 */
 class POINT
 {
public:  
  int16_t x;
  int16_t y;

  POINT() {}
  
#ifdef DEBUG
  void DUMP(const char *szText = 0) const;
#else
  void DUMP(const char *szText = 0) {}
#endif
 };
 
 class RECT 
 {
 public:
  RECT() {}
 
  int16_t left;
  int16_t top;
  int16_t right;
  int16_t bottom;

  int16_t width() const {
    return right - left;
  }
  int16_t height() const {
    return bottom - top;
  }
  void inflate(int16_t px = 1) {
    left -= px;
    top -= px;
    right += px;
    bottom += px;
  }
  void deflate(int16_t px = 1) {
    left += px;
    top += px;
    right -= px;
    bottom -= px;
  }
  bool doesIntersect(const RECT r) const;
  RECT intersect(const RECT r) const;

#ifdef DEBUG
  void DUMP(const char *szText = 0) const;
#else
  void DUMP(const char *szText = 0) {}
#endif
};

/**
 * Extension of ILI9341_t3
 * New functionality - backlight brightness driven by a PWM pin
 */
class Display: public ILI9341_t3 
{
  /* backlight intensity 0..10 */
  byte m_bl = 9;
  
public: 
  Display();

  void setClipRect(const RECT r) {
    _clipx1 = r.left; _clipy1 = r.top; _clipx2 = r.right; _clipy2 = r.bottom;
  }
  RECT getClipRect() const
  {
    RECT r;
    r.left = _clipx1; r.top = _clipy1; r.right = _clipx2; r.bottom = _clipy2;
    return r;
  }
  void resetClipRect() {
    ILI9341_t3::setClipRect();
  }
  

  void fillRect(const RECT r, uint16_t color = ILI9341_BLACK) {
    if(color != ILI9341_BLACK) {
      DEBUG_PRINT("fillRect color:"); DEBUG_PRINTHEX(color); r.DUMP(" r:");
    }
    ILI9341_t3::fillRect(r.left, r.top, r.width(), r.height(), color);
  }

  const ILI9341_t3_font_t *getFont() const {
    return font;
  }

  void setup();

  const int16_t iButtonCornerRadius = 4;
  const uint16_t uButtonBorderColor = ILI9341_DARKGREY;
  const uint16_t uButtonLabelColor = ILI9341_YELLOW;
  const uint16_t uButtonFaceColor = ILI9341_BLACK; // ILI9341_DARKGREEN;

  void drawButton(const RECT rButton, const ILI9341_t3_font_t *pFont, const char *szLabel, bool bEraseBkgnd);

  enum HorizontalAlignment { haLeft, haCenter, haRight };
  enum VerticalAlignment { vaTop, vaCenter, vaBottom };
  
  /**
   * Print text with appropriate alignement in rLocation.
   * rLocation.bottom can be 0 if va == vaTop - in this case rLocation.bottom will be changed/set
   */
  void printText(const char *szText, uint16_t c, uint16_t bg, RECT rLoc, HorizontalAlignment ha, VerticalAlignment va = vaTop, const ILI9341_t3_font_t *pFont = 0, bool bEraseBkgnd = false);
  
  uint16_t getTextColor() const {
    return textcolor;
  }
  const byte BACKLIGHT_MIN = 0;
  const byte BACKLIGHT_MAX = 10;
  /** 
   *  Set backlight intensity
   *  param: bl 0..10
   */  
  void setBacklight(byte bl);
  
  byte getBacklight() {
    return m_bl;
  }
  byte getBacklightMin() {
    return BACKLIGHT_MIN;
  }
  byte getBacklightMax() {
    return BACKLIGHT_MAX;
  }

#ifdef DEBUG
  void DUMP(const char *szText = 0) const;
#else
  void DUMP(const char *szText = 0) {}
#endif
};

extern Display m_lcd;



