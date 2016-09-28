/**
 * Project-specific Views and Dialogs
 */

/** EEPROM address where persistent settings are stored */
const int iEEaddress = 0;

/**
 * Persistent settings to be stored in EEPROM
 */
struct PersistentSettings
{ 
  /** signature */
  union {
  uint16_t m_uSignature;
  char m_signature[2];
  };
  /** Panner slow speed in the direct control screen */
  uint16_t m_uPannerSlowSpeed;
  /** Panner speed in the direct control screen */
  uint16_t m_uPannerFastSpeed;
  /** Panner max speed */
  uint16_t m_uPannerMaxSpeed;
  /** Panner acceleration */
  uint16_t m_uPannerAcceleration;
};

/**
 * Settings view class
 */
class SettingsView : public View
{
  KeyValueListWidget m_settings;
  /** confirmation dialog */
  MessageBox m_resetConfirmation;
  
public:  
  SettingsView();
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  //bool onKeyDown(uint8_t vk);
  bool onKeyAutoRepeat(uint8_t vk);
  /** Long press on central click pops up a Reset confirmation dialog */
  bool onLongKeyDown(uint8_t vk);
  bool onKeyUp(uint8_t vk);

  /** also handles WaypointDefinitionDialog results */
  void onActivate(View *pPrevActive);

  /** reset settings to factory defaults */
  void factoryResetSettings();  
};

 
class WaypointDefinitionDialog : public ModalDialog
{ 
public:  
  ListWidget m_wpoints;
  
  WaypointDefinitionDialog();
  /**
   * to scroll though waypoint list
   */
  bool onKeyUp(uint8_t vk);
  /**
   * to arrange and fill widgets
   */
  void onActivate(View *pPrevActive);
};

/**
 * Direct control view class
 * Use joystick to pan the camera
 * Drop waypoints by pressing middle joystick button - pops up WaypointDefinitionDialog
 */
class ControlView : public View
{
  /** confirmation dialog */
  WaypointDefinitionDialog m_WPontDlg;
  /** extra message to guide the user - displayed at the bottom */
  std::string m_strMessage;
  
public:  
  ControlView();
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  /** start panning */
  bool onKeyDown(uint8_t vk);
  /** start faster panning */
  bool onLongKeyDown(uint8_t vk);
  /** stop panning */
  bool onKeyUp(uint8_t vk);

  /** repaint client area */
  void updateClient(unsigned long now);
  /** 
   * to be called from the main loop on the active view.  
   * Do nothing by default. Return TRUE to update display
   */
  boolean loop(unsigned long now);
  /** also handles WaypointDefinitionDialog results */
  void onActivate(View *pPrevActive);
};

/**
 * Waypoints view class
 * Show the waypoints defined in the Control View
 * Go to the selected waypoint.
 * Delete one by long press of middle key
 */
class WaypointsView : public View
{
  ListWidget m_wpoints;
  //ListSpinnerWidget m_wpoints;
  MessageBox m_deleteConfirmation;
  
public:  
  WaypointsView();
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  bool onKeyDown(uint8_t vk);
  bool onLongKeyDown(uint8_t vk);
  bool onKeyUp(uint8_t vk);

  /**
   * redraw the view
   */
  void updateClient(unsigned long now);
  /**
   * to fill the widgets
   */
  void onActivate(View *pPrevActive);
  boolean loop(unsigned long now);
};

class EditView : public View
{
  KeyValueListWidget m_steps;
  
public:  
  EditView();
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  //bool onKeyDown(uint8_t vk);
  bool onKeyUp(uint8_t vk);
  bool onLongKeyDown(uint8_t vk);

  void onActivate(View *pPrevActive);
private:
  /** go through commands and populate the steps widget */
  void populateWidget(const Command *pCmds, KeyValueListWidget &steps);
  /** walk through the steps widget and save commands */
  bool saveProgram(KeyValueListWidget &steps, Command cmds[]);
};



class RunView : public View
{
  
public:  
  RunView();

  /** analog keyboard APIs where vk is one of VK_xxx */
  bool onKeyUp(uint8_t vk);

  void updateClient(unsigned long now);

  void onActivate(View *pPrevActive);
  /** 
   * to be called from the main loop on the active view.  Do nothing by default. Return TRUE to update display
   */
  boolean loop(unsigned long now);
};

class PausedRunView : public View
{
  
public:  
  PausedRunView();

  /** analog keyboard APIs where vk is one of VK_xxx */
  bool onKeyUp(uint8_t vk);

  void updateClient(unsigned long now);

  void onActivate(View *pPrevActive);
};

class AboutView : public View
{
  ListWidget m_text;
  
public:  
  AboutView();

  /** analog keyboard APIs where vk is one of VK_xxx */
  bool onKeyUp(uint8_t vk);
  void onActivate(View *pPrevActive);
};

extern SettingsView g_settingsView;
extern ControlView g_controlView;
extern WaypointsView g_waypointsView;
extern EditView g_editView;
extern RunView g_runView;
extern PausedRunView g_pausedRunView;
extern AboutView g_aboutView;

