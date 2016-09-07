#include "Panner.h"
#include "BatteryMonitor.h"
#include <font_LiberationSans.h>
#include <font_AwesomeF000.h>
#include <font_AwesomeF100.h>
#include <font_AwesomeF200.h>

/**
 * App-Specific Views (Top Level Windows) Implementation
 */

/**
 * Globals: views
 */
const int iPannerSlowSpeed = 15;
const int iPannerFastSpeed = 3*iPannerSlowSpeed;

SettingsView g_settingsView;
ControlView g_controlView;
WaypointsView g_waypointsView;
EditView g_editView;
RunView g_runView;
PausedRunView g_pausedRunView;
AboutView g_aboutView;

/**
 * 
 */
 
SettingsView::SettingsView() : View("Settings", 
  &AwesomeF000_16, "Z",  // i
  &AwesomeF000_16, "\x7D", // up/down
  0, szOK),
  m_settings(smSingleSelection),
  m_resetConfirmation(szConfirmation, MB_OKCANCEL)
{
  m_settings.hasBorder(false);
  m_resetConfirmation.m_strMessage = "Reset all settings?";
}
  
/** analog keyboard APIs where vk is one of VK_xxx */
/*void SettingsView::onKeyDown(uint8_t vk)
{
}*/

/** Long press on central click pops up a Reset confirmation dialog */
void SettingsView::onLongKeyDown(uint8_t vk)
{
  if(vk == VK_SEL) {
    DEBUG_PRINTLN("SettingsView::onKeyDown(VK_SEL): reset settings!");
    activate(&m_resetConfirmation);
  }
}
void SettingsView::onKeyUp(uint8_t vk)
{
  switch(vk) {
    case VK_LEFT:
    case VK_RIGHT:
      // stop pan
      DEBUG_PRINTLN("SettingsView::onKeyUp(VK_LEFT or VK_RIGHT): stop pan");
      break;
    case VK_SOFTA:
      // switch to About view
      DEBUG_PRINTLN("SettingsView::onKeyUp(VK_SOFTA): switch to About view");
      activate(&g_aboutView);
      break;
    case VK_SOFTB:
      activate((g_pPreviousView == &g_aboutView) ? &g_controlView : g_pPreviousView);
      break;
    case VK_SEL: {
      DEBUG_PRINTLN("SettingsView::onKeyUp(VK_SEL):");
      // move focus to the next editable item
      int16_t iSel = m_settings.getCurSel();
      m_settings.advanceSelection();
      if(iSel == m_settings.getCurSel())
        m_settings.setCurSel(0);
      break;
    }
  }  
}
/** needed to arrange children */
void SettingsView::onPosition()
{
  // support the default behaviour
  View::onPosition();
  DEBUG_PRINTLN("SettingsView::onPosition()");
  // stretch the list box over the entire client area!
  m_settings.setPosition(m_rectClient);
}
/** repaint client area */
void SettingsView::updateClient(unsigned long now)
{
  DEBUG_PRINTLN("SettingsView::updateClient()");
  m_settings.draw();
}

/** also handles m_resetConfirmation results */
void SettingsView::onActivate(View *pPrevActive)
{
  // support the default behaviour
  View::onActivate(pPrevActive);
  DEBUG_PRINTLN("SettingsView::onActivate()");
  if((pPrevActive == &m_resetConfirmation) && (m_resetConfirmation.getResult() == IDOK))
  {
    // the Reset Confirmation MessageBox was just closed.  Reset confirmed!
    
  }
  // clear the listBox
  m_settings.clear();
  // fill m_settings
  {
    char s[80];
    std::string ss;
    m_settings.m_items.push_back(ss); // empty line
    ss = "Direct Control";
    m_settings.m_items.push_back(ss);
    sprintf(s, "  Pan Speed: %i", iPannerSlowSpeed);
    ss = s;
    m_settings.m_items.push_back(ss);
    
    ss = "Runtime";
    m_settings.m_items.push_back(ss);

    sprintf(s, "  Pan Max Speed: %i", (int)g_pPanner->maxSpeed());
    ss = s;
    m_settings.m_items.push_back(ss);
    sprintf(s, "  Pan Acceleration: %i", (int)g_pPanner->getAcceleration());
    ss = s;
    m_settings.m_items.push_back(ss);
    
    m_settings.setCurSel(1);
  }
  g_pPanner->enable(false);
}



/**
 * Waypoint confirmation dialog
 */
WaypointDefinitionDialog::WaypointDefinitionDialog() : 
  ModalDialog("Define WayPoint", MB_OKCANCEL),
  m_wpoints(smSingleSelection)
{
  // set font and text for navigation label
  m_fontNav = &AwesomeF000_16;
  m_szNavLabel = "\x7D";
}

void WaypointDefinitionDialog::onPosition() 
{
  // support the default behaviour
  ModalDialog::onPosition();
  DEBUG_PRINTLN("WaypointDefinitionDialog::onPosition()");

  // stretch the list box over the entire client area - 1/2 of it!
  RECT r = m_rectClient;
  int16_t w = r.width() / 4;
  r.top++;
  r.left += w;
  r.right -= w;
  r.bottom--;
  m_wpoints.setPosition(r); 
}

void WaypointDefinitionDialog::onActivate(View *pPrevActive)
{
  // support the default behaviour
  ModalDialog::onActivate(pPrevActive);

  DEBUG_PRINTLN("WaypointDefinitionDialog::onActivate()");

  // clear the listBox
  m_wpoints.clear();
  // fill m_wpoints from  std::map<std::string, long> g_pPanner->m_wayPoints;
  int i = 0;
  for(auto const& x : g_pPanner->m_wayPoints)
  {
    DEBUG_PRINT("Key="); DEBUG_PRINT(x.first.c_str()); DEBUG_PRINT(" Val="); DEBUG_PRINTDEC(x.second); DEBUG_PRINTLN("");
    char s[80];
    sprintf(s, "%s: %li", x.first.c_str(), x.second);
    std::string ss(s);
    m_wpoints.m_items.push_back(ss);
    i++;
  }
  // add new label
  {
    char s[] = " ";
    s[0] = 'A' + i;
    std::string ss(s);
    m_wpoints.m_items.push_back(ss);
  }
  // and select it by default
  m_wpoints.setCurSel(m_wpoints.m_items.size() - 1);
}

/** 
 *  redraw client area only, not including title and bottom bar 
 *  Refer to m_rectClient for dimensions - changing those in updateClient is not kosher!
 */
void WaypointDefinitionDialog::updateClient(unsigned long now)
{
  DUMP("WaypointDefinitionDialog::updateClient()");
  m_wpoints.DUMP("m_wpoints:");
  
  // we already set m_wpoints.m_position so now we just redraw!
  m_wpoints.draw();
}

void WaypointDefinitionDialog::onKeyUp(uint8_t vk)
{
  switch(vk) {
    case VK_DOWN: 
      m_wpoints.advanceSelection(1);
      break;
    case VK_UP: 
      m_wpoints.advanceSelection(-1);
      break;
    case VK_SEL:
      vk = VK_SOFTB;
      // fall through!
    default:
      ModalDialog::onKeyUp(vk); // to handle OK/Cancel
  }
}

/**
 *  Direct Control View Class Implementation
 */
ControlView::ControlView() : 
  View("Direct Control", &AwesomeF000_16, "\x13",  /* settings icon */  &AwesomeF000_16, "SAT", /* S[T */ 0, "WPoints")
{
}

/** 
 * to be called from the main loop on the active view.  Do nothing by default. Return TRUE to update display
 */
boolean ControlView::loop(unsigned long now)
{
  g_pPanner->runSpeed();
  return (g_pPanner->speed() != 0.0);
}

/** analog keyboard APIs where vk is one of VK_xxx */
void ControlView::onKeyDown(uint8_t vk)
{
  switch(vk) {
    case VK_LEFT:
      // start pan left
      DEBUG_PRINTLN("ControlView::onKeyDown(VK_LEFT): start pan left");
      g_pPanner->setSpeed((float)iPannerSlowSpeed);
      break;
    case VK_RIGHT:
      // start pan right
      g_pPanner->setSpeed((float) - iPannerSlowSpeed);
      DEBUG_PRINTLN("ControlView::onKeyDown(VK_RIGHT): start pan right");
      break;
  }
}

void ControlView::onLongKeyDown(uint8_t vk)
{
  switch(vk) {
    case VK_LEFT:
      // start fast pan left
      DEBUG_PRINTLN("ControlView::onLongKeyDown(VK_LEFT): start fast pan left");
      g_pPanner->setSpeed((float)iPannerFastSpeed);
      break;
    case VK_RIGHT:
      // start fast pan right
      DEBUG_PRINTLN("ControlView::onLongKeyDown(VK_RIGHT): start fast pan right");
      g_pPanner->setSpeed((float) - iPannerFastSpeed);
      break;
  }
}

void ControlView::onKeyUp(uint8_t vk)
{
  switch(vk) {
    case VK_LEFT:
    case VK_RIGHT:
      // stop pan
      DEBUG_PRINTLN("ControlView::onKeyUp(VK_LEFT or VK_RIGHT): stop pan");
      g_pPanner->setSpeed(0);
      break;
    case VK_SOFTA:
      // switch to About view
      DEBUG_PRINTLN("ControlView::onKeyUp(VK_SOFTA): switch to Settings view");
      activate(&g_settingsView);
      break;
    case VK_SOFTB:
      // switch to WayPoints view
      DEBUG_PRINTLN("ControlView::onKeyUp(VK_SOFTB): switch to WayPoints view");
      activate(&g_waypointsView);
      break;
    case VK_SEL:
      DEBUG_PRINTLN("ControlView::onKeyUp(VK_SEL): define a waipoint in the dialog");
      activate(&m_WPontDlg);
      break;
  }  
}

/*void ControlView::onPosition() 
{
  // support the default behaviour
  View::onPosition();
  DEBUG_PRINTLN("ControlView::onPosition()");
  
  m_rectClient.DUMP("m_rectClient: ");
  // reposition children widgets
  m_twMessage.setPosition(m_rectClient.left, m_rectClient.bottom - m_twMessage.getLineSpace(), m_rectClient.right, m_rectClient.bottom);
  m_twMessage.m_position.DUMP("m_twMessage.m_position: ");
}*/

void ControlView::updateClient(unsigned long now)
{
  DEBUG_PRINTLN("ControlView::updateClient()");
  updateClientRunOrPaused(now, false, m_strMessage.c_str());
}

void ControlView::onActivate(View *pPrevActive)
{
  View::onActivate(pPrevActive);
  
  DEBUG_PRINTLN("ControlView::onActivate()");
  if((pPrevActive == &m_WPontDlg) && (m_WPontDlg.getResult() == IDOK))
  {
    // the dialog was just closed, selection confirmed!
    int16_t iSel = m_WPontDlg.m_wpoints.getCurSel();
    if(iSel != LB_ERR) {
      //DEBUG_PRINT("Selected: "); DEBUG_PRINTLN(m_WPontDlg.m_wpoints.m_items[iSel].c_str());
      long cp = g_pPanner->currentPosition();
      //DEBUG_PRINT("CurrentPos: "); DEBUG_PRINTDEC(cp); DEBUG_PRINTLN("");
      std::string strSelectedText = m_WPontDlg.m_wpoints.m_items[iSel];
      std::string str = strSelectedText.substr(0, 1);      
      g_pPanner->m_wayPoints[str] = cp;
    }
    m_strMessage = "Way point defined!";
  }
  else
  {
    m_strMessage = ""; // "Add wpoint by middle click";
  }
  g_pPanner->enable(true);
}

/**
 *  WaypointsView Class Implementation
 */
WaypointsView::WaypointsView() : 
  View("WayPoints", 0, "Control", &AwesomeF000_16, "w[x" /* "\x7D"*/ , 0, "Program"),
  m_wpoints(smSingleSelection),
  m_deleteConfirmation(szConfirmation, MB_OKCANCEL)
{
}

/**
 * analog keyboard APIs where vk is one of VK_xxx 
 */
void WaypointsView::onKeyDown(uint8_t vk)
{
  switch(vk) {
    case VK_SEL: {
      DEBUG_PRINTLN("WaypointsView::onKeyDown(VK_SEL): move the panner to this waypoint!");
      int16_t iSel = m_wpoints.getCurSel();
      if(LB_ERR != iSel) {
        std::string s = m_wpoints.m_items[iSel];
        std::string ss = s.substr(0, 1);
        g_pPanner->moveToWayPoint(ss);
      }
      break;
    }
  }
}

void WaypointsView::onLongKeyDown(uint8_t vk)
{
  switch(vk) {
    case VK_SEL: {
      DEBUG_PRINTLN("WaypointsView::onLongKeyDown(VK_SEL): delete this waypoint?");
      int16_t iSel = m_wpoints.getCurSel();
      if(LB_ERR != iSel) {
        std::string s = m_wpoints.m_items[iSel];
        std::string ss = s.substr(0, 1);
        m_deleteConfirmation.m_strMessage = "Delete the waypoint '" + ss + "'?";
        activate(&m_deleteConfirmation);
      }
      break;
    }
  }
}

void WaypointsView::onKeyUp(uint8_t vk)
{
  switch(vk) {
    case VK_RIGHT: 
    case VK_DOWN: 
      m_wpoints.advanceSelection(1);
      break;
    case VK_LEFT: 
    case VK_UP: 
      m_wpoints.advanceSelection(-1);
      break;

    case VK_SOFTA:
      DEBUG_PRINTLN("WaypointsView::onKeyUp(VK_SOFTA): switch to Control view");
      activate(&g_controlView);
      break;
    case VK_SOFTB:
      // switch to Edit view
      DEBUG_PRINTLN("WaypointsView::onKeyUp(VK_SOFTB): switch to Edit view");
      activate(&g_editView);
      break;
    case VK_SEL:
      DEBUG_PRINTLN("WaypointsView::onKeyUp(VK_SEL)");
      // do nothing here!  
      break;
  }  
}

void WaypointsView::onPosition() 
{
  // support the default behaviour
  View::onPosition();
  DEBUG_PRINTLN("WaypointsView::onPosition()");

  // stretch the list box over the entire client area - 1/2 of it!
  RECT r = m_rectClient;
  int16_t w = r.width() / 3;
  r.left = r.right - w;
  r.top++;
  r.bottom--;
  m_wpoints.setPosition(r);
}

void WaypointsView::updateClient(unsigned long now)
{
  //DEBUG_PRINTLN("WaypointsView::updateClient()");
  RECT rClip = m_rectClient;
  rClip.right -= rClip.width() / 3;
  m_lcd.setClipRect(rClip);
  // do show position and speed
  uint16_t y = m_rectClient.top + m_lcd.fontLineSpace();
  printKeyVal(y, "Pos", g_pPanner->currentPosition());
  y += m_lcd.fontLineSpace() + 2;
  printKeyVal(y, "Speed", (long)g_pPanner->speed());
  
  m_lcd.setClipRect(m_rectClient);
  m_wpoints.draw();
}

void WaypointsView::onActivate(View *pPrevActive)
{
  View::onActivate(pPrevActive);
  DEBUG_PRINTLN("WaypointsView::onActivate()");

  if((pPrevActive == &m_deleteConfirmation) && (m_deleteConfirmation.getResult() == IDOK))
  {
    // the dialog was just closed, deletion confirmed!
    int16_t iSel = m_wpoints.getCurSel();
    if(iSel != LB_ERR) {      
      std::string strSelectedText = m_wpoints.m_items[iSel];
      std::string str = strSelectedText.substr(0, 1);      
      g_pPanner->m_wayPoints.erase(str);
    }
  }
  // clear the listBox
  m_wpoints.clear();
  // fill m_wpoints from  std::map<std::string, long> g_pPanner->m_wayPoints;
  for(auto const& x : g_pPanner->m_wayPoints)
  {
    DEBUG_PRINT("Key="); DEBUG_PRINT(x.first.c_str()); DEBUG_PRINT(" Val="); DEBUG_PRINTDEC(x.second); DEBUG_PRINTLN("");
    char s[80];
    sprintf(s, "%s: %li", x.first.c_str(), x.second);
    std::string ss(s);
    m_wpoints.m_items.push_back(ss);
  }
  g_pPanner->enable(true);
}

/** 
 * to be called from the main loop on the active view.  Do nothing by default. Return TRUE to update display
 */
boolean WaypointsView::loop(unsigned long now)
{
  g_pPanner->run();
  return (g_pPanner->speed() != 0.0);
}

/**
 * EditView class implementation
 */
EditView::EditView() : 
  View("Program", 0, "WPoints", &AwesomeF000_16, "\x7D" /* up/down */, &AwesomeF000_16, "K"/* Run */),
  m_steps(smSingleSelection)
{
  m_steps.hasBorder(false);
}

/** analog keyboard APIs where vk is one of VK_xxx */
void EditView::updateClient(unsigned long now) 
{
  DEBUG_PRINTLN("EditView::updateClient()");
  // draw the content of the program here
  m_steps.draw();
}

void EditView::onKeyDown(uint8_t vk)
{
  DEBUG_PRINT("EditView::onKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}

void EditView::onLongKeyDown(uint8_t vk)
{
  switch(vk)
  {
    case VK_SEL:
      DEBUG_PRINTLN("EditView::onLongKeyDown(VK_SEL): back to settings");
      activate(&g_settingsView);
      break;
    case VK_SOFTA:
      DEBUG_PRINTLN("EditView::onLongKeyDown(VK_SOFTA): back to control");
      activate(&g_controlView);
      break;
    /*default:
      DEBUG_PRINT("EditView::onLongKeyDown ");
      DEBUG_PRINTDEC(vk);
      DEBUG_PRINTLN("");*/
  }
}

void EditView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
    /*case VK_LEFT:
      // decrease the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_LEFT): --");
      break;
    case VK_RIGHT:
      // increase the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_RIGHT): ++");
      break;*/

    case VK_UP:
      // increase the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_UP): ++");
      break;
    case VK_DOWN:
      // decrease the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_DOWN): --");
      break;

      
    case VK_SEL:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_SEL): jump to the next editable field");
      m_steps.advanceSelection();
      break;
    case VK_SOFTA:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_SOFTA): jump to the waypoints view");
      activate(&g_waypointsView);
      break;      
    case VK_SOFTB:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_SOFTB): switch to Run view");
      activate(&g_runView);
      break;
      
    /*default:
      DEBUG_PRINT("EditView::onKeyUp ");
      DEBUG_PRINTDEC(vk);
      DEBUG_PRINTLN("");*/
  }
}

/** needed to arrange children */
void EditView::onPosition()
{
  // support the default behaviour
  View::onPosition();
  DEBUG_PRINTLN("EditView::onPosition()");
  // stretch the list box over the entire client area!
  m_steps.setPosition(m_rectClient);
}

void EditView::onActivate(View *pPrevActive)
{
  // support the default behaviour
  View::onActivate(pPrevActive);
  DEBUG_PRINTLN("EditView::onActivate()");

  g_pPanner->enable(false);  
}

/**
 *  Run View Class Implementation
 */
RunView::RunView() : 
  View("Run", &AwesomeF000_16, "L"/* Pause */, 0, 0, &AwesomeF000_16, "M" /* Stop */)
{
}

/**
 * Globals: commands to run at startup
 */
static Command cmds[] = {
  {chControl, cmdControlBeginLoop, 0, 0},
    {chControl, cmdControlRest,  0, 10000},  // rest for 10 sec
    {chPan,     cmdGoTo, 0, -400},                // go left
    {chControl, cmdControlWaitForCompletion,  0, 50000},  // wait for the movement to be completed for 50 sec
    {chControl, cmdControlRest,  0, 10000},  // rest for 10 sec
    {chPan,     cmdGoTo, 0, 400},                 // go right
    {chControl, cmdControlWaitForCompletion,  0, 50000},  // wait for the movement to be completed for 50 sec
  {chControl, cmdControlEndLoop, 0, 0},
  {chControl, cmdControlNone,    0, 0}
};

 
void RunView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
    /*case VK_SEL:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_SEL)");
      break;
    case VK_LEFT:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_LEFT)");
      break;     
    case VK_RIGHT:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_RIGHT)");
      break;
    case VK_UP:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_UP)");
      break;      
    case VK_DOWN:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_DOWN)");
      break;*/
    case VK_SOFTA:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_SOFTA): switch to Pause view");
      // switch to Pause view
      activate(&g_pausedRunView);
      // and suspend program execution here!
      // ....
      break;
    case VK_SOFTB:
      DEBUG_PRINTLN("RunView::onKeyUp(VK_SOFTB): stop and back to edit");
      // stop execution here!
      // switch to Edit view
      activate(&g_editView);
      // ....
      break;
      
    /*default:
      DEBUG_PRINT("RunView::onKeyUp ");
      DEBUG_PRINTDEC(vk);
      DEBUG_PRINTLN("");*/
  }
}

/**
 *  display Interpreter status
 */
void RunView::updateClient(unsigned long now)
{
  DEBUG_PRINTLN("RunView::updateClient()");
  updateClientRunOrPaused(now, true);
}

void RunView::onActivate(View *pPrevActive)
{
  g_pPanner->enable(true);
  // (re)start command interpreter
  if(g_ci.isPaused())
    g_ci.resumeRun();
  else
    g_ci.beginRun(cmds);
}

/** 
 * to be called from the main loop on the active view.  Do nothing by default. Return TRUE to update display
 */
boolean RunView::loop(unsigned long now)
{
  if(!g_ci.isRunning())
    ;
  else if(g_ci.continueRun(now))
    ;
  else
    g_ci.endRun();
  return needsUpdate(now);
}



/**
 *  Run View Class Implementation
 */
PausedRunView::PausedRunView() : 
  View("Paused", &AwesomeF000_16, "K"/* Run*/, 0, 0, &AwesomeF000_16, "M"/* Stop*/)
{
}

void PausedRunView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
    /*case VK_SEL:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_SEL)");
      break;
    case VK_LEFT:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_LEFT)");
      break;
    case VK_RIGHT:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_RIGHT)");
      break;
    case VK_UP:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_UP)");
      break;
    case VK_DOWN:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_DOWN)");
      break;*/
      
    case VK_SOFTA:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_SOFTA): switch to Run view");
      // resume program execution here!
      // switch to Run view
      activate(&g_runView);
      // ....
      break;
    case VK_SOFTB:
      DEBUG_PRINTLN("PausedRunView::onKeyUp(VK_SOFTB): stop and switch to Run view");
      // stop program execution here!
      // switch to Edit view
      activate(&g_editView);
      // ....
      break;
      
    /*default:
      DEBUG_PRINT("PausedRunView::onKeyUp ");
      DEBUG_PRINTDEC(vk);
      DEBUG_PRINTLN("");*/
  }
}


/**
 *  display Interpreter status
 */
void PausedRunView::updateClient(/*long lPanPos, float flPanSpeed, const char *pLabel, unsigned wSecs,*/ unsigned long now)
{
  DEBUG_PRINTLN("PausedRunView::updateClient()");
  updateClientRunOrPaused(now, true);
}

void PausedRunView::onActivate(View *pPrevActive)
{
  if(g_ci.isRunning())
    g_ci.pauseRun();
  g_pPanner->enable(false);
}

/**
 *  About View Class Implementation
 */
const char *aboutViewLines[] = {
  "",
  "Panner v0.1",
  "(c) 2015-2016 Alex Sokolsky",
  "",
  "Thank you to:",
  "Dean Blackketter for a fork of ILI9341_t3",
  "Paul Stoffregen for ILI9341_t3",
  "Paul Stoffregen for Teensy 3.x",
  "Mike McCauley for AccelStepper",
  "",
  "This software is licensed under the terms",
  "of the GNU Public License, V2."
};

AboutView::AboutView() : 
  View("About", 0, 0, &AwesomeF000_16, "\x7D" /* up/down */, 0, szOK),
  m_text(smNoSelection, &LiberationSans_12)
{
  m_text.hasBorder(false);
  for(size_t i = 0; i <  (sizeof(aboutViewLines)/ sizeof(aboutViewLines[0])); i++) {
    std::string ss(aboutViewLines[i]);
    m_text.m_items.push_back(ss);
  }
}

void AboutView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
    case VK_DOWN: {
      DEBUG_PRINTLN("AboutView::onKeyUp(VK_DOWN)");
      m_text.scroll(1);
      break;
    }
    case VK_UP:
      DEBUG_PRINTLN("AboutView::onKeyUp(VK_UP)");
      m_text.scroll(-1);
      break;
      
    case VK_SOFTA:
    case VK_SOFTB:
      DEBUG_PRINTLN("AboutView::onKeyUp(VK_SOFTB): back to control view");
      activate(g_pPreviousView);
      break;      
  }
}


/**
 *  display About info in a scrollable line list
 */
void AboutView::updateClient(unsigned long now)
{
  //DEBUG_PRINTLN("AboutView::updateClient()");
  m_text.draw();
}

/**
 * to arrange and the widgets
 */
void AboutView::onPosition()
{
  // support the default behaviour
  View::onPosition();
  DEBUG_PRINTLN("AboutView::onPosition()");
  // stretch the list box over the entire client area!
  m_text.setPosition(m_rectClient);
}

void AboutView::onActivate(View *pPrevActive)
{
  // support the default behaviour
  View::onActivate(pPrevActive);
  DEBUG_PRINTLN("AboutView::onActivate()");
  m_text.scroll(-100); // reset first displayed position
  g_pPanner->enable(false);  
}

