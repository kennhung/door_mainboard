void init_lcd(){
  lcd.begin(16, 2);
  // lcd blink 3 times
  for(int i = 0; i < 3; i++) {
    lcd.backlight(); // open backlight
    delay(250);
    lcd.noBacklight(); // close backloght
    delay(250);
  }

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setup Complete");
}

