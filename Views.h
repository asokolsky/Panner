#ifndef Views_h
#define Views_h


 struct RECT {
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
};


class View 
{
protected:  
  //static UTFT m_lcd;
  static Display m_lcd;
  
  const char *m_szTitle;
  const ILI9341_t3_font_t &m_fontSoftA;
  const char *m_szSoftALabel;
  const ILI9341_t3_font_t &m_fontNav;
  const char *m_szNavLabel;
  const ILI9341_t3_font_t &m_fontSoftB;
  const char *m_szSoftBLabel;

  /**  update period 1/2 sec */
  static const unsigned long ulUpdatePeriod = 100;
  /** when the view has to be updated */
  unsigned long m_ulToUpdate = 0;

  static RECT g_rectClient;

public:  
  View(const char *szTitle, const ILI9341_t3_font_t &fontSoftA, const char *szSoftALabel, 
                            const ILI9341_t3_font_t &fontNav, const char *szNavLabel, 
                            const ILI9341_t3_font_t &fontSoftB, const char *szSoftBLabel);
  virtual ~View();

  static View *g_pActiveView;
  
  static void setup();
  static void activate(View *p);

  /** Derived class will overwrite these.  Do nothing by default */
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  virtual void onKeyDown(uint8_t vk);
  virtual void onLongKeyDown(uint8_t vk);
  virtual void onKeyUp(uint8_t vk);

  /** ThumbStick APIs where vk is one of VK_xxx */
  //virtual void onThumbDown() = 0;
  //virtual void onThumbUp() = 0;
  //virtual void onLongThumbDown() = 0;
  //virtual void onThumbStickX(int16_t x) = 0;
  //virtual void onThumbStickY(int16_t y) = 0;

  /** might as well update GUI is its time*/
  virtual void updateMaybe(unsigned long now);
  /** entire screen redraw */
  virtual void update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
  /** 
   *  redraw client area only, not including title and bottom bar 
   *  Refer to g_rectClient for dimensions - changing those in updateClient is not kosher!
   */
  virtual void updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);

protected:
  /** draws the title bar. returns title bar height  */
  int16_t drawTitleBar();
  void drawBattery(uint8_t iPcentFull);
  /** draws the soft labels. returns the bar height  */
  int16_t drawSoftLabels();
  void drawButton(int16_t x, int16_t y, int16_t w, int16_t h, const ILI9341_t3_font_t &font, const char *szLabel);
  void printKeyVal(const char *szKey, long lVal, uint16_t y);
  /**
   *  print text left-aligned in the client area using current font
   */
  void printTextLeft(const char *szText, uint16_t y, const ILI9341_t3_font_t *pFont = 0);
  /**
   *  print text centered in the client area using current font
   */
  void printTextCenter(const char *szText, uint16_t y, const ILI9341_t3_font_t *pFont = 0, int16_t *pDY = 0);
};

class ControlView : public View
{
public:  
  ControlView();
  ~ControlView();
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  void onKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);
  void onLongKeyDown(uint8_t vk);

  void updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
};

class EditView : public View
{
public:  
  EditView();
  ~EditView();
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  void onKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);
  void onLongKeyDown(uint8_t vk);

  void updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
};



class RunView : public View
{
  
public:  
  RunView();
  ~RunView();

  /** analog keyboard APIs where vk is one of VK_xxx */
  //void onKeyDown(uint8_t vk);
  //void onLongKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);

  void updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
};

class PausedRunView : public View
{
  
public:  
  PausedRunView();
  ~PausedRunView();

  /** analog keyboard APIs where vk is one of VK_xxx */
  //void onKeyDown(uint8_t vk);
  //void onLongKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);

  void updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
};

class AboutView : public View
{
  static const char *m_lines[];
  static int16_t m_iLines;
  
  int16_t m_iFirstDisplayedLine = 0;
  
public:  
  AboutView();
  ~AboutView();

  /** analog keyboard APIs where vk is one of VK_xxx */
  //void onKeyDown(uint8_t vk);
  //void onLongKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);

  void updateClient(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
};

extern ControlView g_controlView;
extern EditView g_editView;
extern RunView g_runView;
extern PausedRunView g_pausedRunView;
extern AboutView g_aboutView;

#endif

