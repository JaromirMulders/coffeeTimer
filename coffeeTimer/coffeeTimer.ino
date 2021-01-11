#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // Arduino SPI library

#define cRelayPin    2 
#define cEncoderPinL 3
#define cEncoderPinR 4
#define cEncoderPinB 5


#define clearScreen() tft.fillScreen(ST77XX_BLACK);

#define cScreenSize    240
#define cScreenCenterX 120
#define cScreenCenterY 120


// ST7789 TFT module connections
#define TFT_CS    10  // define chip select pin
#define TFT_DC     9  // define data/command pin
#define TFT_RST    8  // define reset pin, or set to -1 and connect to Arduino RESET pin

// Initialize Adafruit ST7789 TFT library
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

int64_t elapsedTime = 0;
int16_t encoder = 0;
bool    button  = 0;

void setup() {
  Serial.begin(9600);

  tft.init(cScreenSize, cScreenSize, SPI_MODE2);    // Init ST7789 display 240x240 pixel
  tft.setRotation(2);
  clearScreen();

  loadScreen();
    
  pinMode(cRelayPin,OUTPUT);
  pinMode(cEncoderPinL,INPUT_PULLUP);
  pinMode(cEncoderPinR,INPUT_PULLUP);
  pinMode(cEncoderPinB,INPUT_PULLUP);

}

void loop() {
  elapsedTime = millis();


  encoder = readEncoder();
  button  = digitalRead(cEncoderPinB);
  Serial.println(button);
  //delay(100);
}

int readEncoder(){
  static int16_t encoderSum = 0;  
  static bool oldVal = 0;
  
  //check direction that encoder goes in and change value accordingly
  //read first pin to check if encoder has moved
  bool encoderValR = digitalRead(cEncoderPinR);
  //check if encoder has moved and has changed previous  state 
  bool moveCheck = oldVal&(encoderValR^1);
  if(moveCheck){
    bool encoderValL = digitalRead(cEncoderPinL); //check dir with second pin
    int8_t encoderDelta = (encoderValL<<1)-1; //calculate delta with dir
    encoderSum+=encoderDelta; //add delta to encoderval
  }//if
  oldVal = encoderValR;
  
  return encoderSum;
}

void loadScreen(){
  clearScreen();
  
  for(int i = 0; i < 256; i++){
    tft.drawCircle(cScreenCenterX, cScreenCenterY, i+1, ST77XX_RED);  
  }

}//loadScreen
