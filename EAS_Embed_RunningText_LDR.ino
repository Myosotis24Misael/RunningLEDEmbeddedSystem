#include <stdio.h>
#include <RTClib.h>
#include <MD_Parola.h>
#include <SPI.h>
#include "Font_Data.h"
// ->Customize font to fit DOT Matrix interface
// ->Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may need to be adapted
#define ADDR_DS3231 0xD0
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10
//================================================================================================================================

/*Mendeklarasikan variabel-variabel utama yang diperlukan*/
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); // Hardware SPI connection
//                       Arbitrary output pins
RTC_DS3231 rtc; //Deklarasi RTC
int mode=8; //Deklarasi variabel untuk mode pada DOT-MATRIX nantinya, disini mode 8 akan tampil duluan
int saves[7]; //Deklarasi array untuk penyimpanan data Waktu dan Tanggal

/*Menginisiasi atau menjalankan beberapa fungsi utama*/
void setup(void)
{
  rtc.begin(); //Memulai dan menyalakan RTC
  P.begin(); //Memulai dan menyalakan DOT-MATRIX
  P.setFont(dig3x6); //Menginisiasi fungsi setFont untuk merubah Alfabet-Numerik menjadi digit kedalam DOT-MATRIX
  P.displayClear(); //Membersihkan display atau bisa juga untuk membersihkan seluruh zona dalam display saat ini
  P.displayScroll("Misael Randy Limpaty - 0721_18_4000_0056", PA_CENTER, PA_SCROLL_LEFT, 75); //Running text kanan ke kiri
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Mengatur Jam, Menit dan Detik seperti pada laptop/komputer
  pinMode(1,INPUT); //Button 1 - Down
  pinMode(2,INPUT); //Button 2 - Edit atau Setting
  pinMode(3,INPUT); //Button 3 - Up
}
//================================================================================================================================

void loop(void)
{
  P.setIntensity(analogRead(A1)*15/1024); //Membuat sistem terang dan redup otomatis sesuai keadaan dengan LDR
  DateTime now = rtc.now();
  if(mode==0)
  {
    saves[0] = now.hour(); //Memasukkan data Jam
    saves[1] = now.minute(); //Memasukkan data Menit
    saves[2] = now.second(); //Memasukkan data Detik
    saves[3] = now.day(); //Memasukkan data Hari
    saves[4] = now.month(); //Memasukkan data Bulan
    saves[5] = now.year()-0xD0; //Memasukkan data Tahun

    /*Membuat DOT-MATRIX menampilkan DD-MM-YY setiap detik ke 10 s.d 13 setiap menitnya*/
    if(now.second()>9 && now.second()<14)
    {
      P.print(DispOut_DATE(now.day(),now.month(),now.year()-0xD0));
    }
    /*Membuat DOT-MATRIX menampilkan Temperatur Celcius setiap detik ke 14 s.d 17 setiap menitnya*/
    else if(now.second()>13 && now.second()<18)
    {
      P.print(DispOut_TEMPER(rtc.getTemperature()));
    }
    /*Membuat DOT-MATRIX menampilkan Temperatur Celcius selama 5 detik ketika Button 1 ditekan*/
    else if (digitalRead(1))
    {
      P.print(DispOut_DATE(now.day(),now.month(),now.year()-0xD0));
      delay(5000);
    }
    /*Membuat DOT-MATRIX menampilkan HH-MM-SS (Waktu)*/
    else
    {
      P.print(DispOut_TIME(now.hour(),now.minute(),now.second()));
    }
    
    /*Membuat DOT-MATRIX menampilkan Temperatur Celcius selama 5 detik ketika Button 3 ditekan*/
    if (digitalRead(3))
    {
      mode=7; //Mengalihkan program ke mode 7 yang mana menampilkan Temperatur selama 5 detik
      while(digitalRead(3));
    }
    /*Membuat DOT-MATRIX masuk kedalam mode Setting dari User*/
    else if(digitalRead(2)) 
    { 
      mode++; //Berganti kedalam mode Setting HH (Jam)
      while(digitalRead(2)); 
    }
  }
//................................................................................................................................
  /*Membuat HH (Jam) dapat diubah oleh User*/
  else if(mode==1)
  {
    P.print(DispOut_EDIT(saves[0]));
    if(digitalRead(1)) //Menurunkan 1 jam
    {
      saves[0]--; if(saves[0]<0) saves[0]=23; //Ketika jam diturunkan dari 00 maka akan menjadi jam 23
      while(digitalRead(1));
    }
    if(digitalRead(2)) //Berganti kedalam mode Setting MM (Menit)
    {
      mode++; while(digitalRead(2)); 
    }
    if(digitalRead(3)) //Menaikkan 1 jam
    { 
      saves[0]++; if(saves[0]>23) saves[0]=0; //Ketika jam dinaikkan dari 23 maka akan menjadi jam 00
      while(digitalRead(3)); 
    }
  }

  /*Membuat MM (Menit) dapat diubah oleh User*/
  else if(mode==2)
  {
    P.print(DispOut_EDIT(saves[1]));
    if(digitalRead(1)) //Menurunkan 1 menit
    {
      saves[1]--; if(saves[1]<0) saves[1]=59; //Ketika menit diturunkan dari 00 maka akan menjadi menit 59
      while(digitalRead(1));
    }
    if(digitalRead(2)) //Berganti kedalam mode Setting SS (Detik)
    { 
      mode++; while(digitalRead(2)); 
    }
    if(digitalRead(3)) 
    { 
      saves[1]++; if(saves[1]>59) saves[1]=0; //Ketika menit dinaikkan dari 59 maka akan menjadi menit 00
      while(digitalRead(3)); 
    }
  }

  /*Membuat SS (Detik) dapat diubah oleh User*/
  else if(mode==3)
  {
    P.print(DispOut_EDIT(saves[2]));
    if(digitalRead(1)) 
    {
      saves[2]--; if(saves[2]<0) saves[2]=59; //Ketika detik diturunkan dari 00 maka akan menjadi detik 59
      while(digitalRead(1));
    }
    if(digitalRead(2)) 
    { mode++; while(digitalRead(2)); } //Berganti kedalam mode Setting DD (Hari)
    if(digitalRead(3)) 
    { 
      saves[2]++; if(saves[2]>59) saves[2]=0; //Ketika detik dinaikkan dari 59 maka akan menjadi 00
      while(digitalRead(3)); 
    }
  }
//................................................................................................................................
  /*Membuat DD (Hari) dapat diubah oleh User*/
  else if(mode==4)
  {
    P.print(DispOut_EDIT(saves[3]));
    if(digitalRead(1)) 
    {
      saves[3]--; if(saves[3]<1) saves[3]=31;
      while(digitalRead(1));
    }
    if(digitalRead(2)) 
    { mode++; while(digitalRead(2)); }
    if(digitalRead(3)) 
    { 
      saves[3]++; if(saves[3]>30) saves[3]=0;
      while(digitalRead(3)); 
    }
  }

  /*Membuat MM (Bulan) dapat diubah oleh User*/
  else if(mode==5)
  {
    P.print(DispOut_EDIT(saves[4]));
    if(digitalRead(1)) 
    {
      saves[4]--; if(saves[4]<1) saves[4]=12;
      while(digitalRead(1));
    }
    if(digitalRead(2)) 
    { mode++; while(digitalRead(2)); }
    
    if(digitalRead(3)) 
    { 
      saves[4]++; if(saves[4]>12) saves[4]=1;
      while(digitalRead(3)); 
    }
  }

  /*Membuat YY (Tahun) dapat diubah oleh User*/
  else if(mode==6)
  {
    P.print(DispOut_EDIT(saves[5]));
    if(digitalRead(1)) 
    {
      saves[5]--; if(saves[5]<0) saves[5]=99;
      while(digitalRead(1));
    }
    if(digitalRead(2)) 
    { 
      rtc.adjust(DateTime(saves[5], saves[4], saves[3], saves[0], saves[1], saves[2]));
      mode=0; while(digitalRead(2)); 
    }
    if(digitalRead(3)) 
    { 
      saves[5]++; if(saves[5]>99) saves[5]=0;
      while(digitalRead(3));       
    }
  }
//................................................................................................................................
  /*Membuat DOT-MATRIX menampilkan Temperatur Celcius selama 5 detik*/
  else if(mode==7)
  {
    P.print(DispOut_TEMPER(rtc.getTemperature()));
    delay(5000);
    mode=0;
  }
//................................................................................................................................
  /*Membuat DOT-MATRIX menjalankan running text*/
  else if(mode==8)
  {
    if(P.displayAnimate())
    {
      P.displayReset();
      mode=0;
    }
  }
}
//===============================================================================================================================

/*Membuat tampilan waktu HH : MM : SS pada DOT-MATRIX*/
String DispOut_TIME (byte hour, byte minute, byte second) 
{
  String strBuffer = Konversi(hour) + (second%2 ? ' ' : ':') + Konversi(minute) + (second%2 ? ' ' : ':') + Konversi(second);
  //Baris kode diatas bertujuan untuk menampilkan waktu
  //Baris kode diatas juga bertujuan agar ada animasi titik dua ":" kedap-kedip saat waktu berjalan
  char val[9];
  strBuffer.toCharArray(val, 9);
  return String(val);
}

/*Membuat tampilan tanggalan DD - MM - YY pada DOT-MATRIX*/
String DispOut_DATE (byte day, byte month, byte year) 
{
  String strBuffer = Konversi(day) + "-" + Konversi(month) + "-" + Konversi(year);
  //Baris kode diatas bertujuan untuk menampilkan tanggalan
  //Baris kode diatas juga bertujuan untuk memberi pemisah strip "-" pada tanggalan
  char val[9];
  strBuffer.toCharArray(val, 9);
  return String(val);
}

/*Membuat tampilan temperatur Celcius dalam bentuk decimal 1 angka belakang koma*/
String DispOut_TEMPER (double Temperature) 
{
  char temp[5];
  dtostrf(Temperature, 3, 1, temp);
  String strBuffer = "T:" + String(temp) + "*C";
  //Baris kode diatas bertujuan untuk menampilkan suhu dengan format T : XX,X °C
  //Notes: XX,X maksudnya adalah dua digit dengan satu digit dibelakang koma
  char val[10];
  strBuffer.toCharArray(val, 10);
  return String(val);
}

/*Membuat tampilan dalam mode Setting pada DOT-MATRIX*/
String DispOut_EDIT (byte set)
{
  String strBuffer;
  if(mode==1) strBuffer = Konversi(set) + ":  :";
  //Baris kode diatas bertujuan untuk menampilkan HH :   :
  else if(mode==2) strBuffer = "  :" + Konversi(set) + ":";
  //Baris kode diatas bertujuan untuk menampilkan    : MM :  
  else if(mode==3) strBuffer = "  :  :" + Konversi(set);
  //Baris kode diatas bertujuan untuk menampilkan    :    : SS
  else if(mode==4) strBuffer = Konversi(set) + "-  -";
  //Baris kode diatas bertujuan untuk menampilkan DD -   -
  else if(mode==5) strBuffer = "  -" + Konversi(set) + "-";
  //Baris kode diatas bertujuan untuk menampilkan - MM -  
  else if(mode==6) strBuffer = "  -  -" + Konversi(set);
  //Baris kode diatas bertujuan untuk menampilkan -   - YY
  char val[9];
  strBuffer.toCharArray(val, 9);
  return String(val);
}

/*Menambahkan angka 0 dibelakang angka 1 digit DOT-MATRIX*/
String Konversi(int nilai)
{
  if (nilai < 10)
    return "0" + String(nilai);
  if (nilai > 100)
    return "0" + String(nilai-208);
  else return String(nilai);
}
