#ifndef Views_h
#define Views_h

class View 
{
public:  
  View() {}
  virtual ~View() {}

  /** Derived class will overwrite these.  Do nothing by default */
  virtual void setup() = 0;
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  //virtual void onKeyDown(uint8_t vk) = 0;
  //virtual void onKeyUp(uint8_t vk) = 0;
  //virtual void onLongKeyDown(uint8_t vk) = 0;

  /** ThumbStick APIs where vk is one of VK_xxx */
  virtual void onThumbDown() = 0;
  virtual void onThumbUp() = 0;
  virtual void onLongThumbDown() = 0;
  virtual void onThumbStickX(int16_t x) = 0;
  virtual void onThumbStickY(int16_t y) = 0;
};

extern View *g_pView;

class SimpleView : public View
{
public:  
  SimpleView();
  ~SimpleView();

  void setup();
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  //void onKeyDown(uint8_t vk);
  //void onKeyUp(uint8_t vk);
  //void onLongKeyDown(uint8_t vk);

  /** ThumbStick APIs where vk is one of VK_xxx */
  void onThumbDown();
  void onThumbUp();
  void onLongThumbDown();
  void onThumbStickX(int16_t x);
  void onThumbStickY(int16_t y);


private:  
};

#endif

