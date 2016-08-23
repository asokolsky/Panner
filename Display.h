class Display: public ILI9341_t3 
{
public: 
  Display();

  const ILI9341_t3_font_t *getFont() {
    return font;
  }

  void setup();

};


