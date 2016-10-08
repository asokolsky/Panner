#include "Panner.h"
#include "BatteryMonitor.h"
//#include <EEPROM.h>
#include <font_LiberationSans.h>
#include <font_AwesomeF000.h>
#include <font_AwesomeF080.h>
#include <font_AwesomeF100.h>
#include <font_AwesomeF200.h>

/**
 * App-Specific Views (Top Level Windows) Implementation
 */

/**
 * Globals: views
 */
SettingsView g_settingsView;
ControlView g_controlView;
WaypointsView g_waypointsView;
EditView g_editView;
RunView g_runView;
PausedRunView g_pausedRunView;
AboutView g_aboutView;

/**
 * Globals: commands to run at startup
 */
static Command cmds[] = 
{
  {chPan,     cmdSetMaxSpeed, 50},
  {chPan,     cmdSetAcceleration, 10},
  {chControl, cmdControlBeginLoop, 0},
    {chPan,     cmdGoToWaypoint, 1},                 // go right
    {chControl, cmdControlWaitForCompletion,  50000},  // wait for the movement to be completed for 50 sec
    {chControl, cmdControlRest,  10000},  // rest for 10 sec
    {chPan,     cmdGoToWaypoint, 2},                // go left
    {chControl, cmdControlWaitForCompletion,  50000},  // wait for the movement to be completed for 50 sec
    {chControl, cmdControlRest,  10000},  // rest for 10 sec
  {chControl, cmdControlEndLoop, 0},
  {chControl, cmdControlNone,    0}
};

static const char szRest[] = "Rest";
static const char szWaitForCompletion[] = "Wait";
static const char szBeginLoop[] = "Begin Loop";
static const char szEndLoop[] = "End Loop";
static const char szGo[] = "Go";
static const char szGoTo[] = "GoTo";
static const char szSetMaxSpeed[] = "Max Speed";
static const char szSetAcceleration[] = "Acceleration";

static const char szPanSlowSpeed[] = "Pan Slow Speed";
static const char szPanMaxSpeed[] = "Pan Max Speed";
static const char szPanAcceleration[] = "Pan Acceleration";

/**
 * 
 */
 
SettingsView::SettingsView() : 
  View("Settings", &AwesomeF000_16, "Z"/* i */,  &AwesomeF000_16, "[\x7D"/* up/down */, 0, szOK),
  //View("Settings", &AwesomeF000_16, "Z"/* i */,  &AwesomeF080_16, "\x5C", 0, szOK),
  m_resetConfirmation(szConfirmation, MB_OKCANCEL)
{
  m_settings.hasBorder(false);
  addChild(&m_settings);
  m_resetConfirmation.m_strMessage = "Reset all settings?";
}

/** Long press on central click pops up a Reset confirmation dialog */
bool SettingsView::onLongKeyDown(uint8_t vk)
{
  if(vk == VK_SEL) {
    DEBUG_PRINTLN("SettingsView::onKeyDown(VK_SEL): reset settings!");
    activate(&m_resetConfirmation);
    return true;
  }
  return false;
}

bool SettingsView::onKeyAutoRepeat(uint8_t vk)
{
  switch(vk) {
    case VK_UP: {
      DEBUG_PRINTLN("SettingsView::onKeyAutoRepeat(VK_UP)");
      //m_settings.setCurValue(m_settings.getCurValue() + 1);
      ListSpinnerWidget *p = m_settings.getCurValue();
      if(p != 0)
        p->advanceSelection(1);
      break;
    }
    case VK_DOWN: {
      DEBUG_PRINTLN("SettingsView::onKeyAutoRepeat(VK_DOWN)");
      //m_settings.setCurValue(m_settings.getCurValue() - 1);
      ListSpinnerWidget *p = m_settings.getCurValue();
      if(p != 0)
        p->advanceSelection(-1);
      break;
    }
    default:
      return false;
  }  
  return true;
}

bool SettingsView::onKeyUp(uint8_t vk)
{
  switch(vk) {
    case VK_LEFT:
      DEBUG_PRINTLN("SettingsView::onKeyUp(VK_LEFT): jump to a prev editable field");
      m_settings.advanceSelection(-1);
      break;
    case VK_RIGHT:
      DEBUG_PRINTLN("SettingsView::onKeyUp(VK_RIGHT or VK_SEL): jump to a next editable field");
      m_settings.advanceSelection(1);
      break;
    case VK_SEL:
      DEBUG_PRINTLN("SettingsView::onKeyUp(VK_SEL):");
      // move focus to the next editable item
      if(m_settings.advanceSelection() == LB_ERR) {
        m_settings.setCurSel(0);
        m_settings.advanceSelection();
      }
      break;
      
    case VK_UP: {
      DEBUG_PRINTLN("SettingsView::onKeyUp(VK_UP)");
      //m_settings.setCurValue(m_settings.getCurValue() + 1);
      ListSpinnerWidget *p = m_settings.getCurValue();
      if(p != 0)
        p->advanceSelection(1);
      break;
    }
    case VK_DOWN: {
      DEBUG_PRINTLN("SettingsView::onKeyUp(VK_DOWN)");
      //m_settings.setCurValue(m_settings.getCurValue() - 1);
      ListSpinnerWidget *p = m_settings.getCurValue();
      if(p != 0)
        p->advanceSelection(-1);
      break;
    }
    case VK_SOFTA:
      // switch to About view
      DEBUG_PRINTLN("SettingsView::onKeyUp(VK_SOFTA): switch to About view");
      activate(&g_aboutView);
      break;
    case VK_SOFTB: {
      // save settings!     
      g_settings.m_uPannerSlowSpeed = m_settings.getNumericValue(szPanSlowSpeed);
      g_settings.m_uPannerFastSpeed = 3 * g_settings.m_uPannerSlowSpeed;
      g_pPanner->setMaxSpeed(g_settings.m_uPannerMaxSpeed = m_settings.getNumericValue(szPanMaxSpeed));
      g_pPanner->setAcceleration(g_settings.m_uPannerAcceleration = m_settings.getNumericValue(szPanAcceleration));
      g_settings.save();
      //    
      activate(((g_pPreviousView == &g_aboutView) || (g_pPreviousView == &m_resetConfirmation)) ? 
               &g_controlView : 
               g_pPreviousView);
      break;
    }
    default:
      return false;
  }  
  return true;
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
    g_settings.factoryReset();
    g_pPanner->setMaxSpeed(g_settings.m_uPannerMaxSpeed);
    g_pPanner->setAcceleration(g_settings.m_uPannerAcceleration);
  }
  // clear the listBox
  m_settings.clear();
  // fill m_settings
  {
    m_settings.push_back(" Direct Control");
    m_settings.push_back(szPanSlowSpeed, (long)g_settings.m_uPannerSlowSpeed);
    m_settings.push_back(" Runtime");
    m_settings.push_back(szPanMaxSpeed, (long)g_pPanner->getMaxSpeed());
    m_settings.push_back(szPanAcceleration, (long)g_pPanner->getAcceleration());
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
  addChild(&m_wpoints);
  // set font and text for navigation label
  m_fontNav = &AwesomeF000_16;
  m_szNavLabel = "\x7D";
}

/** 
 * fill m_wpoints from  std::map<std::string, long> g_pPanner->m_wayPoints; 
 * szName is the name of the selected waypoint if not NULL
 */
static int16_t fill(ListWidget &list, const std::map<std::string, long> &wayPoints, const char szName[4])
{
  char szWaypoint[5];
  if(szName == 0)
    szWaypoint[0] = '\0';
  else {
    memcpy(szWaypoint, szName, 4);
    szWaypoint[4] = 0;
  }
  // clear the listBox
  list.clear();
  int16_t i = 0;
  for(auto const& x : wayPoints)
  {
    DEBUG_PRINT("Key="); DEBUG_PRINT(x.first.c_str()); DEBUG_PRINT(" Val="); DEBUG_PRINTDEC(x.second); DEBUG_PRINTLN("");
    char s[80];
    sprintf(s, "%s: %li", x.first.c_str(), x.second);
    list.push_back(s);
    if(x.first.compare(szWaypoint) == 0)
      list.setCurSel(i);
    i++;
  }
  return i;
}

static bool newWaypointLabel(char szLabel[2], const std::map<std::string, long> &wayPoints)
{
  for(char ch = 'A'; ch < 'Z'; ch++)
  {
    szLabel[0] = ch;
    if(wayPoints.count(szLabel) == 0)
      return true;    
  }
  return false;
}

void WaypointDefinitionDialog::onActivate(View *pPrevActive)
{
  // support the default behaviour
  ModalDialog::onActivate(pPrevActive);
  DEBUG_PRINTLN("WaypointDefinitionDialog::onActivate()");

  // stretch the list box over the half of the client area
  RECT r = m_rectClient;
  int16_t w = r.width() / 4;
  r.top++;
  r.left += w;
  r.right -= w;
  r.bottom--;
  m_wpoints.setPosition(r); 

  // fill ListWidget m_wpoints from  std::map<std::string, long> g_pPanner->m_wayPoints;
  fill(m_wpoints, g_pPanner->m_wayPoints, 0);
  // add new label
  {
    char s[] = " ";
    newWaypointLabel(s, g_pPanner->m_wayPoints);
    m_wpoints.m_items.push_back(s);
  }
  // and select it by default
  m_wpoints.setCurSel(m_wpoints.m_items.size() - 1);
}

bool WaypointDefinitionDialog::onKeyUp(uint8_t vk)
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
      return ModalDialog::onKeyUp(vk); // to handle OK/Cancel
  }
  return true;
}

/**
 *  Direct Control View Class Implementation
 */
ControlView::ControlView() :
  View("Direct Control", &AwesomeF000_16, "\x13",  /* settings icon */  &AwesomeF000_16, "SAT" /* S[T */, 0, "WPoints")
{
}

/** 
 * To be called from the main loop on the active view.  
 * Do nothing by default. Return TRUE to update display
 */
boolean ControlView::loop(unsigned long now)
{
  g_pPanner->runSpeed();
  return (g_pPanner->speed() != 0.0);
}

/** analog keyboard APIs where vk is one of VK_xxx */
bool ControlView::onKeyDown(uint8_t vk)
{
  switch(vk) {
    case VK_LEFT:
      // start pan left
      DEBUG_PRINTLN("ControlView::onKeyDown(VK_LEFT): start pan left");
      g_pPanner->setSpeed((float)g_settings.m_uPannerSlowSpeed);
      break;
    case VK_RIGHT:
      // start pan right
      g_pPanner->setSpeed((float) - g_settings.m_uPannerSlowSpeed);
      DEBUG_PRINTLN("ControlView::onKeyDown(VK_RIGHT): start pan right");
      break;
    default:
      return false;
  }
  return true;
}

bool ControlView::onLongKeyDown(uint8_t vk)
{
  switch(vk) {
    case VK_LEFT:
      // start fast pan left
      DEBUG_PRINTLN("ControlView::onLongKeyDown(VK_LEFT): start fast pan left");
      g_pPanner->setSpeed((float)g_settings.m_uPannerFastSpeed);
      break;
    case VK_RIGHT:
      // start fast pan right
      DEBUG_PRINTLN("ControlView::onLongKeyDown(VK_RIGHT): start fast pan right");
      g_pPanner->setSpeed((float) - g_settings.m_uPannerFastSpeed);
      break;
    default:
      return false;
  }
  return true;
}

bool ControlView::onKeyUp(uint8_t vk)
{
  switch(vk) {
    case VK_LEFT:
    case VK_RIGHT:
      // stop pan
      DEBUG_PRINTLN("ControlView::onKeyUp(VK_LEFT or VK_RIGHT): stop pan");
      g_pPanner->setSpeed(0);
      break;
    case VK_SOFTA:
      // switch to Settings view
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
    default:
      return false;
  }
  return true;
}

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
      
      m_strMessage = "Way point '" + str + "' defined!";
    }
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
  //m_wpoints(smSingleSelection),
  m_deleteConfirmation(szConfirmation, MB_OKCANCEL)
{
  addChild(&m_wpoints);
}

/**
 * analog keyboard APIs where vk is one of VK_xxx 
 */
bool WaypointsView::onKeyDown(uint8_t vk)
{
  switch(vk) {
    case VK_SEL: {
      DEBUG_PRINTLN("WaypointsView::onKeyDown(VK_SEL): move the panner to this waypoint!");
      int16_t iSel = m_wpoints.getCurSel();
      if(LB_ERR != iSel) {
        std::string s = m_wpoints.m_items[iSel];
        std::string ss = s.substr(0, 1);
        g_pPanner->moveToWayPoint(ss.c_str());
      }
      break;
    }
    default:
      return false;
  }
  return true;
}

bool WaypointsView::onLongKeyDown(uint8_t vk)
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
    default:
      return false;
  }
  return true;
}

bool WaypointsView::onKeyUp(uint8_t vk)
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
    default:
      return false;
  }
  return true;
}

void WaypointsView::updateClient(unsigned long now)
{
  View::updateClient(now);
  //DEBUG_PRINTLN("WaypointsView::updateClient()");
  uint16_t x = m_rectClient.width() / 4;             // position of the first ':'
  RECT rClip = m_rectClient;
  rClip.right -= rClip.width() / 3;
  m_lcd.setClipRect(rClip);
  // do show position and speed
  uint16_t y = m_rectClient.top + m_lcd.fontLineSpace();
  printKeyVal(x, y, "Pan", g_pPanner->currentPosition());
  y += m_lcd.fontLineSpace() + 2;
  printKeyVal(x, y, "Speed", (long)g_pPanner->speed());
  
  m_lcd.setClipRect(m_rectClient);
  //m_wpoints.draw();
}

void WaypointsView::onActivate(View *pPrevActive)
{
  View::onActivate(pPrevActive);
  DEBUG_PRINTLN("WaypointsView::onActivate()");

  // stretch the list box over the entire client area - 1/2 of it!
  RECT r = m_rectClient;
  int16_t w = r.width() / 3;
  r.left = r.right - w;
  r.top++;
  r.bottom--;
  m_wpoints.setPosition(r);
  

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
  // fill ListWidget m_wpoints from  std::map<std::string, long> g_pPanner->m_wayPoints;
  fill(m_wpoints, g_pPanner->m_wayPoints, 0);

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
  View("Program", 0, "WPoints", &AwesomeF000_16, "G" /* up/down */, &AwesomeF000_16, "K"/* Run */),
  m_steps()
{
  m_steps.hasBorder(false);
  addChild(&m_steps);
}

/*void EditView::onKeyDown(uint8_t vk)
{
  DEBUG_PRINT("EditView::onKeyDown ");
  DEBUG_PRINTDEC(vk);
  DEBUG_PRINTLN("");
}*/

bool EditView::onLongKeyDown(uint8_t vk)
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
    default:
      //DEBUG_PRINT("EditView::onLongKeyDown "); DEBUG_PRINTDEC(vk);  DEBUG_PRINTLN("");
      return false;
  }
  return true;
}

bool EditView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
    case VK_UP: {
      // increase the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_UP)");
      //m_steps.setCurValue(m_steps.getCurValue() + 1);      
      ListSpinnerWidget *p = m_steps.getCurValue();
      if(p != 0)
        p->advanceSelection(1);
      break;
    }
    case VK_DOWN: {
      // decrease the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_DOWN)");
      //m_steps.setCurValue(m_steps.getCurValue() - 1);
      ListSpinnerWidget *p = m_steps.getCurValue();
      if(p != 0)
        p->advanceSelection(-1);
      break;
    } 
    case VK_LEFT:
      // decrease the editable field value
      DEBUG_PRINTLN("EditView::onKeyUp(VK_LEFT): jump to a prev editable field");
      m_steps.advanceSelection(-1);
      break;
    case VK_RIGHT:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_RIGHT or VK_SEL): jump to a next editable field");
      m_steps.advanceSelection(1);
      break;
    case VK_SEL:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_RIGHT or VK_SEL): jump to a next editable field");
      if(m_steps.advanceSelection(1) == LB_ERR)
        m_steps.setCurSel(0);
      break;
    case VK_SOFTA:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_SOFTA): jump to the waypoints view");
      activate(&g_waypointsView);
      break;      
    case VK_SOFTB:
      DEBUG_PRINTLN("EditView::onKeyUp(VK_SOFTB): switch to Run view");
      // save the program
      if(saveProgram(m_steps, cmds))
      {
        activate(&g_runView);
      }
      else
      {
        DEBUG_PRINTLN("EditView::onKeyUp(VK_SOFTB): Can't run this!");
        ;
      
      }
      break;      
    default:
      //DEBUG_PRINT("EditView::onKeyUp "); DEBUG_PRINTDEC(vk); DEBUG_PRINTLN("");
      return false;
  }
  return true;
}

/** 
 * go through commands and populate the steps widget 
 */
void EditView::populateWidget(const Command *pCmds, KeyValueListWidget &steps)
{
  steps.clear();
  
  for(;pCmds != 0; pCmds++)
  {
    switch(pCmds->m_channel)
    {
      case chControl:
        switch(pCmds->m_command)
        {
          case cmdControlNone:
            // graceful completion!
            return;
          case cmdControlRest:
            steps.push_back(szRest, pCmds->m_uValue / 1000);
            break;
          case cmdControlWaitForCompletion:
            steps.push_back(szWaitForCompletion, pCmds->m_uValue / 1000);
            break;
          case cmdControlBeginLoop:
            steps.push_back(szBeginLoop);
            break;
          case cmdControlEndLoop:
            steps.push_back(szEndLoop);
            break;
          default:
            pCmds->DUMP("populateWidget() ABNORMAL EXIT in chControl!");
            return;
        }
        break;
      case chPan:
        switch(pCmds->m_command)
        {
          case cmdControlNone:
            // graceful completion!
            return;
          case cmdGo:
            steps.push_back(szGo, pCmds->m_lPosition);
            break;
          case cmdGoTo:
            steps.push_back(szGoTo, pCmds->m_lPosition);
            break;
          case cmdGoToWaypoint: {
            ListSpinnerWidget wayPoints;
            // fill ListWidget m_wpoints from  std::map<std::string, long> g_pPanner->m_wayPoints;
            fill(wayPoints, g_pPanner->m_wayPoints, pCmds->m_szParam);
            steps.push_back(szGoTo, wayPoints);
            break;
          }
          case cmdSetMaxSpeed:
            steps.push_back(szSetMaxSpeed, pCmds->m_uValue);
            break;
          case cmdSetAcceleration:
            steps.push_back(szSetAcceleration, pCmds->m_uValue);
            break;
          default:
            pCmds->DUMP("populateWidget() ABNORMAL EXIT in chPan!");
            return;
        }
        break;
      default:
        pCmds->DUMP("populateWidget() ABNORMAL EXIT in unknown channel!");
        return;
    }    
  }  
}

/** walk through the steps widget and save commands */
bool EditView::saveProgram(KeyValueListWidget &steps, Command cmds[])
{
  for(size_t i = 0; i < m_steps.m_items.size(); i++)
  {
    switch(cmds[i].m_channel)
    {
      case chControl:
        switch(cmds[i].m_command)
        {
          case cmdControlNone:
            // graceful completion!
            return true;
          case cmdControlRest:
          case cmdControlWaitForCompletion:
            cmds[i].m_uValue = (unsigned long)steps.getNumericValue((int16_t)i);
            break;
          case cmdControlBeginLoop:
            //
            break;
          case cmdControlEndLoop:
            //
            break;
          default:
            cmds[i].DUMP("saveProgram() ABNORMAL EXIT in chControl - unknown command!");
            return false;
        }
        break;
      case chPan:
        switch(cmds[i].m_command)
        {
          case cmdControlNone:
            // graceful completion!
            return true;
          case cmdGo:
          case cmdGoTo:
            cmds[i].m_lPosition = (long)steps.getNumericValue((int16_t)i);
            break;
          case cmdGoToWaypoint: {
            ListSpinnerWidget *p = &steps.m_values[i];
            int16_t iSel = p->getCurSel();
            if((iSel < 0) || (iSel >= (int16_t)p->m_items.size()))
              return false;
            cmds[i].m_szParam[0] = p->m_items[iSel][0];
            cmds[i].m_szParam[1] = '\0';
            break;
          }
          case cmdSetMaxSpeed:
          case cmdSetAcceleration:
            cmds[i].m_lPosition = (unsigned long)steps.getNumericValue((int16_t)i);
            break;
          default:
            cmds[i].DUMP("populateWidget() ABNORMAL EXIT in chPan - unknown command!");
            return false;
        }
        break;
      default:
        cmds[i].DUMP("saveProgram() ABNORMAL EXIT - unknown channel!");
        return false;
    }    
  }
  return true;
}

void EditView::onActivate(View *pPrevActive)
{
  // support the default behaviour
  View::onActivate(pPrevActive);
  DEBUG_PRINTLN("EditView::onActivate()");

  // fill the m_steps from cmds
  populateWidget(cmds, m_steps);

  g_pPanner->enable(false);  
}

/**
 *  Run View Class Implementation
 */
RunView::RunView() : 
  View("Run", &AwesomeF000_16, "L"/* Pause */, 0, 0, &AwesomeF000_16, "M" /* Stop */)
{
}

bool RunView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
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
    default:
      return false;
  }
  return true;
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

bool PausedRunView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
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
    default:
      return false;
  }
  return true;
}


/**
 *  display Interpreter status
 */
void PausedRunView::updateClient(unsigned long now)
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
  addChild(&m_text);
  for(size_t i = 0; i <  (sizeof(aboutViewLines)/ sizeof(aboutViewLines[0])); i++)
    m_text.m_items.push_back(aboutViewLines[i]);
}

bool AboutView::onKeyUp(uint8_t vk)
{
  switch(vk)
  {
    case VK_DOWN:
      DEBUG_PRINTLN("AboutView::onKeyUp(VK_DOWN)");
      m_text.scroll(1);
      break;
    case VK_UP:
      DEBUG_PRINTLN("AboutView::onKeyUp(VK_UP)");
      m_text.scroll(-1);
      break;
    case VK_SOFTA:
    case VK_SOFTB:
      DEBUG_PRINTLN("AboutView::onKeyUp(VK_SOFTB): back to control view");
      activate(g_pPreviousView);
      break;      
    default:
      return false;
  }
  return true;
}

void AboutView::onActivate(View *pPrevActive)
{
  // support the default behaviour
  View::onActivate(pPrevActive);
  DEBUG_PRINTLN("AboutView::onActivate()");
  
  m_text.scroll(-100); // reset first displayed position
  g_pPanner->enable(false);  
}

