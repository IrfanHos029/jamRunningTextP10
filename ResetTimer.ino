void Reset(){
  now = RTC.now();
  int jam = now.hour();
  int menit = now.minute();
  int detik = now.second();
Serial.println(menit);

  if(jam == 23 && menit == 20 && detik == 00){
    Buzzer(1);
    delay(1000);
    digitalWrite(reset,LOW);
  }
}
