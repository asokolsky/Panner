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

  /** reset settings to factory defaults */
  void factoryReset();
  /** restore g_settings from EEPROM */
  bool restore();
  /** save g_settings into EEPROM */
  bool save();

 
};

extern PersistentSettings g_settings;

