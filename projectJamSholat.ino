// 18/04/2021

#include <SPI.h>
#include <DMD3asis.h>
#include <font/KecNumber.h>
#include <font/BigNumber.h>
#include <font/Font4x6.h>
#include <font/SystemFont5x7.h>
#include <font/Font3x5.h>
#include <font/EMSans8x16 .h>


#include <DS3231.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <MemoryFree.h>

#define BUZZ    2 

#define Font0 Font4x6
#define Font3 BigNumber
#define Font2 Font3x5
#define Font1 SystemFont5x7
#define Font4 KecNumber
#define Font5 EMSans8x16 
    
// Object Declarations
DMD3 Disp(2,1);
char *pasar[] ={"Wage", "Kliwon", "Legi", "Pahing", "Pon"}; 
char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};
char *mounthJawa[]= {"Muharram","Shafar","Rab.awal","Rab.akhir","Jum.awal","Jum.akhir","Rajab","Sya'ban","Ramadhan","Syawal","Dzulqa'dah","Dzulhijah"};
char *sholatCall[] = {"IMSAK","SUBUH","TERBIT","DHUHA","DUHUR","ASHAR","MAGRIB","ISYA","JUM'AT"};                                            
int maxday[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
RTClib          RTC;
DS3231          Clock;

//Structure of Variable 
typedef struct  // loaded to EEPROM
  {
    uint8_t state;   // 1 byte  add 0
    float   L_LA;    // 4 byte  add 1
    float   L_LO;    // 4 byte  add 5
    float   L_AL;    // 4 byte  add 9
    float   L_TZ;    // 4 byte  add 13
    uint8_t MT;      // 1 byte  add 17  // value 1-masjid  2-mushollah 3-surau 4-langgar 
    uint8_t BL;      // 1 byte  add 18
    uint8_t IH;      // 1 byte  add 19
    uint8_t SO;      // 1 byte  add 20
    uint8_t JM;      // 1 byte  add 21
    uint8_t I1;      // 1 byte  add 22
    uint8_t I4;      // 1 byte  add 23
    uint8_t I5;      // 1 byte  add 24
    uint8_t I6;      // 1 byte  add 25
    uint8_t I7;      // 1 byte  add 26
    uint8_t BZ;      // 1 byte  add 27
    uint8_t SI;      // 1 byte  add 28
    uint8_t ST;      // 1 byte  add 29
    uint8_t SU;      // 1 byte  add 30
    int8_t  CH;      //20 1 byte  add 31
    int8_t  II;      //21 1 byte  add 32
    int8_t  IS;      //22 1 byte  add 33
    int8_t  IL;      //23 1 byte  add 34
    int8_t  IA;      //24 1 byte  add 35
    int8_t  IM;      //25 1 byte  add 36
  } struct_param;
  
typedef struct  
  { 
    uint8_t   hD;
    uint8_t   hM;
    uint16_t  hY;
  } hijir_date;

   float latitude = -7.238816115709593;
   float longitude = 112.75318149040366;
//   float latitude = 7.14;
//   float longitude = 112.45;
   float timezone = +7;
   
 // Variable by Structure     
struct_param    Prm;
hijir_date      nowH;   

// Time Variable
DateTime        now;
float           floatnow   = 0;
uint8_t         daynow     = 0;
uint8_t         ty_puasa   = 0;
uint8_t         hd_puasa   = 0; 
int8_t          SholatNow  = -1;
boolean         jumat      = false;
boolean         azzan      = false;
uint8_t         reset_x    = 0;   
uint8_t         reset      = 3;

//Other Variable
float sholatT[8]  = {0,0,0,0,0,0,0,0};
uint8_t Iqomah[8] = {0,0,0,0,0,0,0,0};

//Blue tooth Pram Receive
char        CH_Prm[155];
int         DWidth  = Disp.width();
int         DHeight = Disp.height();
boolean     DoSwap;
int         RunSel    = 1; //
int         RunFinish = 0 ;

  static uint16_t saveTime = 0;
  bool stateBlink=false;

//=======================================
//===SETUP=============================== 
//=======================================
void setup()
  { //init comunications 
    Wire.begin();
    Serial.begin(9600);
     digitalWrite(reset,HIGH);
     pinMode(reset,OUTPUT);
     pinMode(BUZZ, OUTPUT); 
     Buzzer(0);
  //RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Get Saved Parameter from EEPROM   
    updateTime();

    //init P10 Led Disp & Salam
    Disp_init();
  }

//=======================================
//===MAIN LOOP Function =================   
//=======================================
void loop()
  { 
    // Reset & Init Display State
    update_All_data();
    check_azzan();  //check Sholah Time for Azzan
    Reset();
    DoSwap  = false ;
    fType(5);  
    Disp.clear();
    
    // Timer Function every 10 Minutes
    // Up All function with Timer in this fuction
  //  Timer_Minute(1);

 //Serial.print((float)sholatT[0]);
    // =========================================
    // List of Display Component Block =========
    // =========================================

//    anim_JG(1);            // addr: 1 show date time
//    drawSholat(1);  
    runningTextOut(drawDayDate(),75,1); 
    runningTextOut(drawTextOut(),75,2);                             // addr: 2 show Masjid Name
    
   // addr: 5 show Remander Puasa
//    drawSholat(2);                                              // addr: 5 show sholat time
//    dwMrq(drawTextOut()    ,75,1,1);                             // addr: 6 show Info 1
//   // anim_DT(7);                                                 // addr: 7 show date time    
//    dwMrq(drawInfo(280)   ,75,2,8);                             // addr: 8 show Info 2
//    drawSholat(9);                                              // addr: 9 show sholat time
//    dwMrq(drawInfo(430)   ,75,1,10);                            // addr: 10 show Info 3
//
///    dwMrq(drawDayDate()   ,75,1,3);                             // addr: 3 show Hijriah date
 //   dwMrq(msgPuasa(hd_puasa,ty_puasa),75,0,4); 
    drawAzzan(100);    // addr: 100 show Azzan
    runningAfterAdzan(101);
   // drawImsak(101);                                            // addr: 101 show Iqomah
//    dwMrq(drawInfo(580),50,0,102); //Message Sholat biasa       // addr: 202 show Message Sholah
//    dwMrq(drawInfo(730),50,0,103); //Message Sholat jumat       // addr: 203 show Message Jum'at
 //   blinkBlock(104);                                            // addr: 104 show Blink  Sholat    

    // =========================================
    // Display Control Block ===================
    // =========================================
    if(RunFinish==1) {RunSel = 2; RunFinish =0;}                      //after anim 1 set anim 2
    if(RunFinish==2) {RunSel = 1; RunFinish =0;}                      //after anim 2 set anim 3
//    if(RunFinish==3) {RunSel = 4; RunFinish =0;}
//    if(RunFinish==3)                                                  //after anim 3 set anim 5 or anim 4 if puasa
//         {
//          if (ty_puasa!=0)  {RunSel = 4; RunFinish =0;}
//          else {RunSel = 1; RunFinish =0;}
//         }
//    if(RunFinish==4)  {RunSel = 1;  RunFinish =0;}                      //after anim 4 set anim 5
//    if(RunFinish==5)  {RunSel = 6;  RunFinish =0;}                      //after anim 5 set anim 6
//    if(RunFinish==6)  {RunSel = 8;  RunFinish =0;}                      //after anim 6 set anim 7
//  //  if(RunFinish==7)  {RunSel = 8;  RunFinish =0;}                      //after anim 7 set anim 8
//    if(RunFinish==8)  {RunSel = 9;  RunFinish =0;}                      //after anim 8 set anim 9
//    if(RunFinish==9)  {RunSel = 10; RunFinish =0;}                      //after anim 9 set anim 10
//    if(RunFinish==10) {RunSel = 1;  RunFinish =0;}                      //after anim 10 set anim 1
    
    
//    if(RunFinish==100 and jumat )     {RunSel = 103; RunFinish = 0; reset_x = 1;}  //after Azzan Jumat (anim 100)
//    else if(RunFinish==100)           {RunSel = 101; RunFinish =0;}               //after Azzan Sholah (Iqomah)
        
    if(RunFinish==100) {RunSel = 101; RunFinish =0;}         
    if(RunFinish==101) {RunSel = 1; RunFinish =0;}                  
//    if(RunFinish==103) {RunSel = 104; RunFinish =0;}                  //after Messagw Jum'at (anim 103) set Blink Sholah(anim 104)
//    if(RunFinish==104) {RunSel = 1; RunFinish =0;}                    //after Blink Sholah back to anim 1 

    // =========================================
    // Swap Display if Change===================
    // =========================================
    if(DoSwap){Disp.swapBuffers();} // Swap Buffer if Change
  }


// =========================================
// DMD3 P10 utility Function================
// =========================================
void Disp_init() 
  { Disp.setDoubleBuffer(true);
    Timer1.initialize(2000);
    Timer1.attachInterrupt(scan);
    setBrightness(200);
    fType(1);  
    Disp.clear();
    Disp.swapBuffers();
    }

void setBrightness(int bright)
  { Timer1.pwm(9,bright);}

void scan()
  { Disp.refresh();}
  
// =========================================
// Time Calculation Block===================
// =========================================
void updateTime()
  { now = RTC.now();
    floatnow = (float)now.hour() + (float)now.minute()/60 + (float)now.second()/3600;
    daynow   = Clock.getDoW();    // load day Number
  }
  
/*void Timer_Minute(int repeat_time) //load every  1 minute
  { 
    static uint16_t   lsRn;
    uint16_t          Tmr = millis();
    if((Tmr-lsRn)>(repeat_time*60000))
      {
        lsRn =Tmr;
        update_All_data();
        Serial.print("freeMemory()=");Serial.println(freeMemory()); 
        SendPrm();       
      }
  }
*/
void update_All_data()
  {
  uint8_t   date_cor = 0;
  updateTime();
  sholatCal();                                                // load Sholah Time                                         // check jadwal Puasa Besok
  if(floatnow>00.00) {date_cor = 1;}                     // load Hijr Date + corection next day after Mhagrib 
  nowH = toHijri(now.year(),now.month(),now.day(),date_cor);  // load Hijir Date
  
//  if ((floatnow > (float)21) or (floatnow < (float)3.5) )    {setBrightness(15);}
//      else                                                   {setBrightness(Prm.BL);}  
//  }
  }
  
    
void check_azzan()
  { //Check Waktu Sholat
    SholatNow  = -1;
    for(int i=0; i <=7; i++)
      {
        if (i!=0 and i!=2 and i!=3)  // bukan terbit dan bukan dhuha
          {
            if(floatnow >= sholatT[i])
              {
                SholatNow = i;
                if(!azzan and (floatnow > sholatT[i]) and (floatnow < (sholatT[i]+0.03))) 
                  { 
                    azzan =true;
                    RunSel = 100;
                  }  
              }
          }
      }
  }
