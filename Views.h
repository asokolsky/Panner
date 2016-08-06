#ifndef Views_h
#define Views_h

class View 
{
protected:  
  static UTFT m_lcd;
  const char *m_szTitle;

  /**  update period 1/2 sec */
  static const unsigned long ulUpdatePeriod = 100;
  /** when the view was updated */
  unsigned long m_ulUpdated = 0;

public:  
  View(const char *szTitle);
  virtual ~View();

  static View *g_pActiveView;
  
  static void setup();
  
  static void activate(View *p) {
    g_pActiveView = p;
  }

  /** Derived class will overwrite these.  Do nothing by default */
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  //virtual void onKeyDown(uint8_t vk) = 0;
  //virtual void onKeyUp(uint8_t vk) = 0;
  //virtual void onLongKeyDown(uint8_t vk) = 0;

  /** ThumbStick APIs where vk is one of VK_xxx */
  virtual void onThumbDown() = 0;
  virtual void onThumbUp() = 0;
  //virtual void onLongThumbDown() = 0;
  virtual void onThumbStickX(int16_t x) = 0;
  virtual void onThumbStickY(int16_t y) = 0;

  /** might as well update GUI is its time*/
  virtual void updateMaybe(unsigned long now);
  virtual void update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);

protected:
  void drawTitleBar();

private:  
  void drawBattery(int16_t x, int16_t y, uint8_t iPcentFull);
};

class RunView : public View
{
  
public:  
  RunView();
  ~RunView();

  /** analog keyboard APIs where vk is one of VK_xxx */
  //void onKeyDown(uint8_t vk);
  //void onKeyUp(uint8_t vk);
  //void onLongKeyDown(uint8_t vk);

  /** ThumbStick APIs where vk is one of VK_xxx */
  void onThumbDown();
  void onThumbUp();
  //void onLongThumbDown();
  void onThumbStickX(int16_t x);
  void onThumbStickY(int16_t y);

  void update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
};

class EditView : public View
{
public:  
  EditView();
  ~EditView();
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  //void onKeyDown(uint8_t vk);
  //void onKeyUp(uint8_t vk);
  //void onLongKeyDown(uint8_t vk);

  /** ThumbStick APIs where vk is one of VK_xxx */
  void onThumbDown();
  void onThumbUp();
  //void onLongThumbDown();
  void onThumbStickX(int16_t x);
  void onThumbStickY(int16_t y);

  void update(long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs, unsigned long now);
};

extern RunView g_runView;
extern EditView g_editView;

#endif

