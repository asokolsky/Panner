#ifndef Views_h
#define Views_h

class View 
{
protected:  
  //static UTFT m_lcd;
  static ILI9341_t3 m_lcd;
  
  const char *m_szTitle;
  const char *m_szSoftALabel;
  const char *m_szNavLabel;
  const char *m_szSoftBLabel;

  /**  update period 1/2 sec */
  static const unsigned long ulUpdatePeriod = 100;
  /** when the view was updated */
  unsigned long m_ulUpdated = 0;

public:  
  View(const char *szTitle, const char *szSoftALabel, const char *szNavLabel, const char *szSoftBLabel);
  virtual ~View();

  static View *g_pActiveView;
  
  static void setup();
  static void activate(View *p);

  /** Derived class will overwrite these.  Do nothing by default */
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  virtual void onKeyDown(uint8_t vk) = 0;
  virtual void onKeyUp(uint8_t vk) = 0;
  virtual void onLongKeyDown(uint8_t vk) = 0;

  /** ThumbStick APIs where vk is one of VK_xxx */
  //virtual void onThumbDown() = 0;
  //virtual void onThumbUp() = 0;
  //virtual void onLongThumbDown() = 0;
  //virtual void onThumbStickX(int16_t x) = 0;
  //virtual void onThumbStickY(int16_t y) = 0;

  /** might as well update GUI is its time*/
  virtual void updateMaybe(unsigned long now);
  virtual void update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);

protected:
  void drawTitleBar();
  void printKeyVal(const char *szKey, long lVal, uint16_t y);

  void drawBattery(uint8_t iPcentFull);
  void drawSoftLabels();
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

  void update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
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

  void update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
};



class RunView : public View
{
  
public:  
  RunView();
  ~RunView();

  /** analog keyboard APIs where vk is one of VK_xxx */
  void onKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);
  void onLongKeyDown(uint8_t vk);

  void update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
};

class PausedRunView : public View
{
  
public:  
  PausedRunView();
  ~PausedRunView();

  /** analog keyboard APIs where vk is one of VK_xxx */
  void onKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);
  void onLongKeyDown(uint8_t vk);

  void update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
};


extern ControlView g_controlView;
extern EditView g_editView;
extern RunView g_runView;
extern PausedRunView g_pausedRunView;

#endif

