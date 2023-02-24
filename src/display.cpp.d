// #include "config.hpp" 
// #include "iostream" 
U8G2_SH1106_128X64_NONAME_F_HW_I2C o_display(OLED_ROTATION);

Display::Display() { 
    u8g2_SetI2CAddress(&o_display, DISPLAY_ADRESS);  
}

void Display::begin() { 
    o_display.begin();
    o_display.setFontMode(0); 
    o_display.setDrawColor(1); 
    o_display.setFont(u8g2_font_profont11_mf); 
    o_display.clearBuffer();
    o_display.clearDisplay();
    o_display.clearWriteError();
    o_display.sendBuffer();
    o_display.flush();   
}
 

void Display::printTimeToDisplay(int second, int minute, int hour) {   
    char buff[9];  
    sprintf(buff, "%02d:%02d:%02d", hour, minute, second); 
    Serial.print("Current Time: "); 
    Serial.println(buff); 

    o_display.clearBuffer(); 
    o_display.setFont(u8g2_font_profont22_mf);
    int strWidth    = o_display.getStrWidth(buff) / 2 ;
    int strHeight   = o_display.getMaxCharHeight() / 2 - 5;

    o_display.drawStr(128/2-strWidth, 64/2 + strHeight, buff);

    o_display.setFont(u8g2_font_profont11_mf);  
    strHeight = o_display.getMaxCharHeight() / 2 - 25;  

    o_display.drawStr(128/2-strWidth, 64/2 + strHeight, "Current Time:");  
    try
    {
        o_display.sendBuffer(); 
    }
    catch(const std::exception& e)
    {
        Serial.println(e.what());
    }
      
} 