#ifndef Views_h
#define Views_h

/**
 * This is a NANO-sized windowing system for Arduino with TFT displays.
 * Low level support provided by ILI9341_t3 or rather the fork of it by blackketter
 * For sample of use see Views2.h
 */

#include "Display.h"
#include "Widgets.h"


/**
 * View is a widget with a title and bottom bar.  
 * Kinda like Application or Top Level window in Windows.
 */
class View : public Widget
{
protected:
  static Stepper *g_pPanner;
  
  const char *m_szTitle;
  const ILI9341_t3_font_t *m_fontSoftA;
  const char *m_szSoftALabel;
  const ILI9341_t3_font_t *m_fontNav;
  const char *m_szNavLabel;
  const ILI9341_t3_font_t *m_fontSoftB;
  const char *m_szSoftBLabel;

  /**  update period 1/2 sec */
  static const unsigned long ulUpdatePeriod = 100;
  /** when the view has to be updated */
  unsigned long m_ulToUpdate = 0;

  bool needsUpdate(unsigned long now) {
    return (now >= m_ulToUpdate);
  }

  View *m_zParent = 0;
  View *m_zChild = 0;

  /*View *getParent() {
    return m_zParent;
  }
  void setParent(View *p) {
    m_zParent = p;
  }*/

public:  
  View(const char *szTitle, const ILI9341_t3_font_t *fontSoftA, const char *szSoftALabel, 
                            const ILI9341_t3_font_t *fontNav, const char *szNavLabel, 
                            const ILI9341_t3_font_t *fontSoftB, const char *szSoftBLabel);
  
  /** The Active View */
  static View *g_pActiveView;
  
  /** once in application lifetime initialization */  
  static void setup();
  
  /** Activate the View.  Will call onDeActivate and onActivate */
  static void activate(View *p);
  /** view DEactivation call-back */
  virtual void onDeActivate(View *pAboutToBeActive);
  /** view activation call-back */
  virtual void onActivate(View *pPrevActive);

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
  
  /** to be called from the main loop on the active view.  Do nothing by default. return TRUE to update the display.  */
  virtual bool loop(unsigned long now);

  /** recalc the client rect */
  void onPosition();


  /** might as well update GUI is its time*/
  void updateMaybe(unsigned long now);
  /** entire screen redraw */
  void update(unsigned long now);

  /** 
   *  redraw client area only, not including title and bottom bar 
   *  Refer to g_rectClient for dimensions - changing those in updateClient is not kosher!
   */
  virtual void updateClient(unsigned long now);

  void DUMP(const char *szText = 0);

protected:

  /** draws the title bar. */
  virtual void drawTitleBar();
  void drawBattery(uint8_t iPcentFull);
  /** draws the soft labels. */
  void drawSoftLabels();
  void drawButton(int16_t x, int16_t y, int16_t w, int16_t h, const ILI9341_t3_font_t *pFont, const char *szLabel);
  void printKeyVal(uint16_t y, const char *szKey1, long lVal1, const char *szKey2 = 0, long lVal2 = 0);

  /** 
   * updateClient implementation for Control, Run or Paused view 
   */
  void updateClientRunOrPaused(unsigned long now, bool bExtendedInfo, const char *pMsg = 0);

};


const uint16_t MB_OK               =0x0000;
const uint16_t MB_OKCANCEL         =0x0001;
/*
const uint16_t MB_ABORTRETRYIGNORE =0x0002;
const uint16_t MB_YESNOCANCEL      =0x0003;
const uint16_t MB_YESNO            =0x0004;
const uint16_t MB_RETRYCANCEL      =0x0005;
const uint16_t MB_CANCELTRYCONTINUE=0x0006;
const uint16_t MB_HELP             =0x4000;
*/
/** ModalDialog results */
const int16_t IDUNKNOWN=0;
const int16_t IDOK=1;
const int16_t IDCANCEL=2;
/*
const int16_t IDABORT=3;
const int16_t IDRETRY=4;
const int16_t IDIGNORE=5;
const int16_t IDYES=6;
const int16_t IDNO=7;
const int16_t IDTRYAGAIN=10;
const int16_t IDCONTINUE=11;
*/

class ModalDialog : public View
{
  /**  one of MB_xxx */
  uint16_t m_uType;
  /**  one of IDxxx */
  uint16_t m_iRes = IDUNKNOWN;
    
public:  
  ModalDialog(const char *szTitle, 
    const ILI9341_t3_font_t *fontSoftA, const char *szSoftALabel, 
    const ILI9341_t3_font_t *fontNav, const char *szNavLabel, 
    const ILI9341_t3_font_t *fontSoftB, const char *szSoftBLabel);
  /**
   * uType is one of MB_xxx
   */
  ModalDialog(const char *szTitle, uint16_t uType = MB_OK);
  /** Modal Dialog has a different title bar style */
  void drawTitleBar();

  /**
   * VK_SOFTA - Cancels
   * VK_SOFTB - Affirms
   */
  void onKeyUp(uint8_t vk);

  void onActivate(View *pPrevActive);

  int16_t getResult() {
    return m_iRes;
  }
  bool wasCancelled() {
    return (m_iRes == IDCANCEL);
  }
  
};

extern const char szCancel[];
extern const char szOK[];


#endif

