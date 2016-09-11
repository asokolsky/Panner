
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
  void DUMP(const char *szText = 0);
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

  int16_t width() {
    return right - left;
  }
  int16_t height() {
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

#ifdef DEBUG
  void DUMP(const char *szText = 0);
#else
  void DUMP(const char *szText = 0) {}
#endif
};


class Display: public ILI9341_t3 
{
public: 
  Display();

  void setClipRect(RECT &r) {
    _clipx1 = r.left; _clipy1 = r.top; _clipx2 = r.right; _clipy2 = r.bottom;
  }
  void getClipRect(RECT &r) {
    r.left = _clipx1; r.top = _clipy1; r.right = _clipx2; r.bottom = _clipy2;
  }
  void resetClipRect() {
    ILI9341_t3::setClipRect();
  }
  

  void fillRect(RECT &r, uint16_t color = ILI9341_BLACK) {
    if(color != ILI9341_BLACK) {
      DEBUG_PRINT("fillRect color:"); DEBUG_PRINTHEX(color); r.DUMP(" r:");
    }
    ILI9341_t3::fillRect(r.left, r.top, r.width(), r.height(), color);
  }

  /*void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color = ILI9341_BLACK) {
    ILI9341_t3::fillRect(x, y, w, h, color);
  }*/

  const ILI9341_t3_font_t *getFont() {
    return font;
  }

  void setup();

  const int16_t iButtonCornerRadius = 4;
  const uint16_t uButtonBorderColor = ILI9341_DARKGREY;
  const uint16_t uButtonLabelColor = ILI9341_YELLOW;
  const uint16_t uButtonFaceColor = ILI9341_BLACK; // ILI9341_DARKGREEN;

  void drawButton(RECT rButton, const ILI9341_t3_font_t *pFont, const char *szLabel, bool bEraseBkgnd);

#ifdef DEBUG
  void DUMP(const char *szText = 0);
#else
  void DUMP(const char *szText = 0) {}
#endif
};

extern Display m_lcd;



