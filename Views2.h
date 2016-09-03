/**
 * Project-specific Views and Dialogs
 */
 
 
class WaypointDefinitionDialog : public ModalDialog
{ 
public:  
  ListWidget m_wpoints;
  
  WaypointDefinitionDialog();
  /**
   * to scroll though waypoint list
   */
  void onKeyUp(uint8_t vk);
  /**
   * to arrange and fill widgets
   */
  void onActivate(View *pPrevActive);
  /**
   * to arrange and fill widgets
   */
  void onPosition();
  
  void updateClient(unsigned long now);
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
  void onKeyDown(uint8_t vk);
  /** start faster panning */
  void onLongKeyDown(uint8_t vk);
  /** stop panning */
  void onKeyUp(uint8_t vk);

  /** needed to arrange children */
  //void onPosition();
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

class WaypointDeleteConfirmationDialog : public ModalDialog
{ 
public:  
  std::string m_strMessage;
  
  WaypointDeleteConfirmationDialog();
  /**
   * to scroll though waypoint list
   */
  //void onKeyUp(uint8_t vk);
  /**
   * to arrange and fill widgets
   */
  //void onActivate(View *pPrevActive);
  /**
   * to arrange and fill widgets
   */
  //void onPosition();
  
  void updateClient(unsigned long now);
};

class WaypointsView : public View
{
  //std::string m_msg;
  ListWidget m_wpoints;
  WaypointDeleteConfirmationDialog m_deleteConfirmation;
  
public:  
  WaypointsView();
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  void onKeyDown(uint8_t vk);
  void onLongKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);

  /**
   * to arrange and the widgets
   */
  void onPosition();
  void updateClient(unsigned long now);
  /**
   * to fill the widgets
   */
  void onActivate(View *pPrevActive);
  boolean loop(unsigned long now);
};

class EditView : public View
{
public:  
  EditView();
  
  /** analog keyboard APIs where vk is one of VK_xxx */
  void onKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);
  void onLongKeyDown(uint8_t vk);

  void updateClient(unsigned long now);

  void onActivate(View *pPrevActive);
};



class RunView : public View
{
  
public:  
  RunView();

  /** analog keyboard APIs where vk is one of VK_xxx */
  //void onKeyDown(uint8_t vk);
  //void onLongKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);

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
  //void onKeyDown(uint8_t vk);
  //void onLongKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);

  void updateClient(unsigned long now);

  void onActivate(View *pPrevActive);
};

class AboutView : public View
{
  static const char *m_lines[];  
  static int16_t m_iFirstDisplayedLine; // = 0;
  
public:  
  AboutView();
  //~AboutView();

  /** analog keyboard APIs where vk is one of VK_xxx */
  //void onKeyDown(uint8_t vk);
  //void onLongKeyDown(uint8_t vk);
  void onKeyUp(uint8_t vk);

  void onActivate(View *pPrevActive);

  void updateClient(unsigned long now);
};

extern ControlView g_controlView;
extern WaypointsView g_waypointsView;
extern EditView g_editView;
extern RunView g_runView;
extern PausedRunView g_pausedRunView;
extern AboutView g_aboutView;

