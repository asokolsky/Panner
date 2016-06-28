#ifndef Views_h
#define Views_h

class View 
{
public:  
  View() {}
  virtual ~View() {}

  /** Derived class will overwrite these.  Do nothing by default */
  virtual void setup() = 0;
  virtual void onKeyDown(uint8_t vk) = 0;
  virtual void onKeyUp(uint8_t vk) = 0;
  virtual void onLongKeyDown(uint8_t vk) = 0;
};

extern View *g_pView;

class SimpleView : public View
{
public:  
  SimpleView();
  ~SimpleView();

  void setup();
  void onKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);
  void onLongKeyDown(uint8_t vk);
private:  
};

#endif

