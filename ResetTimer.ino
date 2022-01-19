void Reset(){
  now = RTC.now();
  int jam = now.hour();
  int menit = now.minute();
  int detik = now.second();
Serial.print(jam);
Serial.print(":");
Serial.print(menit);
Serial.print(":");
Serial.println(detik);

  if(jam == 0 && menit == 0 && detik == 0){
    Buzzer(1);
    delay(1000);
    digitalWrite(reset,LOW);
  }
}
