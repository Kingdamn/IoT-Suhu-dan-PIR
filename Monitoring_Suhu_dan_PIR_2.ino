#define BLYNK_TEMPLATE_ID "TMPL6G7MSrpE1"
#define BLYNK_TEMPLATE_NAME "Suhu dan PIR"
#define BLYNK_AUTH_TOKEN "hFJc3DV1OuFkTxldC-MmzcJICOVhv21K"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char ssid[] = "TECNO";
char pass[] = "0987654321";
char auth[] = BLYNK_AUTH_TOKEN;

int relay = 14;
int PIR = 4;
int statusPin = 0;

#define DHTPIN 5 //pin DATA konek ke pin 5 Arduino
#define DHTTYPE DHT22 //tipe sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

WidgetLED kipas(V3);
WidgetLED obyek(V2);

float humi, temp;//deklarasi variabel

void setup()
{
  Serial.begin(9600); //baud 9600
  dht.begin();
  Wire.begin(0, 2);
  lcd.begin();
  lcd.backlight();

  pinMode(relay, OUTPUT);
  pinMode(PIR, INPUT);
  digitalWrite(relay, HIGH);
  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    //progress sedang mencari WiFi
    Serial.print("."); lcd.setCursor(12, 3); lcd.print(".");
    delay(500);
  }
  Serial.println("Wifi Connected");
  lcd.setCursor(0, 0); lcd.print("WiFi Connected"); //lcd.setCursor(0,0);lcd.print("WiFi Connected");
  Serial.println("IP Address : "); lcd.setCursor(0, 1); lcd.print("IP : "); //lcd.setCursor(0,1);lcd.print("IP : ");
  Serial.println(WiFi.localIP()); lcd.setCursor(4, 1); lcd.print(WiFi.localIP()); //lcd.setCursor(5,1);lcd.print(WiFi.localIP());
  delay(500); lcd.clear();
}

void loop()
{

  Blynk.run();
  humi = dht.readHumidity();//baca kelembaban
  temp = dht.readTemperature();//baca suhu
  Blynk.virtualWrite(V0, temp);  // kirim data suhu ke blynk
  Blynk.virtualWrite(V1, humi);  // kirim data kelembapan ke blynk

  lcd.setCursor(0, 0); lcd.print("Suhu : "); lcd.print(temp); lcd.print(" C");
  lcd.setCursor(0, 1); lcd.print("Humi : "); lcd.print(humi); lcd.print(" %");

  statusPin = digitalRead(PIR);
  if (statusPin == HIGH)
  {
    Serial.print("Gerakan Terdeteksi"); obyek.on(); delay(1000);
    Blynk.logEvent("gerakan_hama",String("Ada hama pada gabah"));
  }
  else
  {
    Serial.print("Tidak ada Gerakan"); obyek.off();
  }


  if (isnan(humi) || isnan(temp)) { //jika tidak ada hasil
    Serial.println("DHT22 tidak terbaca... !");
    return;
  }
  else { //jika ada hasilnya
    Serial.print("Suhu=");  //kirim serial "Suhu"
    Serial.print(temp);     //kirim serial nilai suhu
    Serial.println("C");    //kirim serial "C" Celcius
    Serial.print("Humi=");  //kirim serial "Humi"
    Serial.print(humi);     //kirim serial nilai kelembaban
    Serial.println("%");  //kirim serial "%RH"

    if (temp > 40) {
      digitalWrite(relay, LOW);
      kipas.on();
    }
    if (temp < 40) {
      digitalWrite(relay, HIGH);
      kipas.off();
    }
  }
  delay(1000);            //tunda 1 detik untuk pembacaan berikutnya
}
