#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // Arduino SPI library

#define cRandomPin  A0
#define cRelayPin    2 
#define cEncoderPinL 3
#define cEncoderPinR 4
#define cEncoderPinB 5

#define cDotAmnt 16

#define cScreenSize    240
#define cScreenCenterX 120
#define cScreenCenterY 120

// ST7789 TFT module connections
#define TFT_CS    10  // define chip select pin
#define TFT_DC     9  // define data/command pin
#define TFT_RST    8  // define reset pin, or set to -1 and connect to Arduino RESET pin

#define clearScreen() tft.fillScreen(ST77XX_BLACK);

// Initialize Adafruit ST7789 TFT library
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

unsigned long elapsedTime = 0;
int16_t encoder = 0;
bool    button  = 0;

bool grindState = 0;
unsigned int grindTimer = 0;

//create array for all position of dots for the grind animation
int16_t dotPos[cDotAmnt][2];
int16_t dotSize[cDotAmnt];

void setup() {
  Serial.begin(9600);

  tft.init(cScreenSize, cScreenSize, SPI_MODE2);    // Init ST7789 display 240x240 pixel
  tft.setRotation(2);
  clearScreen();

  loadScreen();
  setGrindAnimation();

  randomSeed(analogRead(cRandomPin));

  pinMode(cRandomPin,INPUT);
  pinMode(cRelayPin,OUTPUT);
  pinMode(cEncoderPinL,INPUT_PULLUP);
  pinMode(cEncoderPinR,INPUT_PULLUP);
  pinMode(cEncoderPinB,INPUT_PULLUP);

  digitalWrite(cRelayPin,HIGH);

}

void loop() {
  elapsedTime = millis();

  encoder = readEncoder();
  button  = readbutton();
  //Serial.println(encoder);
  startGrind();

  showTime();

  if(!grindState){
    
  }else{
    grindAnimation();
  }

  delay(0);

}

void showTime(){
  static int16_t oldVal = 0;
  tft.setTextSize(5);


  if(encoder != oldVal){
    tft.setCursor(cScreenCenterX-50, cScreenCenterX-20);
    tft.setTextColor(ST77XX_BLACK);
    tft.print(oldVal);
    
    oldVal = encoder;
  }
  
  tft.setCursor(cScreenCenterX-50, cScreenCenterX-20);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(encoder);  

}

void startGrind(){

  if(button){
    grindState^=1;
    if(!grindState) clearScreen();
  }else{
    
  }
  
}//startGrind

bool readbutton(){
  static unsigned long sinceButtonPressed = 0;
  static bool buttonSwitch = 0;
  
  bool buttonPressed = digitalRead(cEncoderPinB)^1;  
  bool buttonVal = 0;
  //if button s pressed
  if(buttonPressed){
    //check how much time has passed since last click
    unsigned long buttonCheck = elapsedTime-sinceButtonPressed;
    //if button was off and more than ~50ms has passed set buttonval to 0
    if(buttonSwitch && buttonCheck > 50) buttonVal = 1;
    //set switch 0 and wait till button has been released before buttonVal can be set to 1 again
    buttonSwitch = 0;
    //reset timer for button time check
    sinceButtonPressed = millis();
  }else{
    buttonSwitch = 1;
  }
  
  return buttonVal;
}//readButton

int16_t readEncoder(){
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
}//readButton

void loadScreen(){
  clearScreen();
  
  for(int i = 0; i < 50; i++){
    tft.drawCircle(cScreenCenterX, cScreenCenterY, i+1, ST77XX_RED);  
  }

}//loadScreen

void setGrindAnimation(){ 
   clearScreen();
   //get random value from noise from not connected analogPin
   for(int8_t i = 0; i < cDotAmnt; i++){
     dotPos[i][0] = random(10,cScreenSize-10); 
     dotPos[i][1] = random(0,cScreenSize); 
     dotSize[i]   = random(2,5); 
     
   }//for
}

void grindAnimation(){
  
   for(int8_t i = 0; i < cDotAmnt; i++){
     //erase old dots
     tft.fillCircle(dotPos[i][0], dotPos[i][1]+dotSize[i], dotSize[i], ST77XX_BLACK);
     //move dots downwards in random motion
     dotPos[i][0]+=random(-3,3);
     dotPos[i][1]++;
     //wrap around use number 255 becease its more effiecent and also adds a bit more randomness
     dotPos[i][0]&=255;
     dotPos[i][1]&=255;     
   
     tft.fillCircle(dotPos[i][0], dotPos[i][1]+dotSize[i], dotSize[i], ST77XX_WHITE);
   }//for

}    
