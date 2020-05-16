//   This is Graphical Engineering Project CS 209
//   System Case with 16 x 16 Equalizer
// =======================================================================

#include <Adafruit_NeoPixel.h>
#include <si5351mcu.h>    //Si5351mcu library
Si5351mcu Si;             //Si5351mcu Board
#define PULSE_PIN     13 
#define NOISE         50
#define ROWS          16  //Кол-во в высоту
#define COLUMNS       16  //Кол-во в ширину
#define DATA_PIN      9   //led data pin
#define STROBE_PIN    6   //MSGEQ7 strobe pin
#define RESET_PIN     7   //MSGEQ7 reset pin
#define NUMPIXELS    ROWS * COLUMNS
struct Point{
char x, y;
char  r,g,b;
bool active;
};
struct TopPoint{
int position;
int peakpause;
};
Point spectrum[ROWS][COLUMNS];
TopPoint peakhold[COLUMNS];
int spectrumValue[COLUMNS];
long int counter = 0;
int long pwmpulse = 0;
bool toggle = false;
int long time_change = 0;
int effect = 0;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, DATA_PIN, NEO_GRB + NEO_KHZ800);

void setup() 
 {
 Si.init(25000000L);
 Si.setFreq(0, 104570);
 Si.setFreq(1, 166280);
 Si.setPower(0, SIOUT_8mA);
 Si.setPower(1, SIOUT_8mA);
 Si.enable(0);
 Si.enable(1);
 pinMode      (STROBE_PIN,    OUTPUT);
 pinMode      (RESET_PIN,     OUTPUT);
 pinMode      (DATA_PIN,      OUTPUT);
 pinMode      (PULSE_PIN,     OUTPUT); 
 digitalWrite(PULSE_PIN, HIGH);
 delay(100);
 digitalWrite(PULSE_PIN, LOW);
 delay(100);
 digitalWrite(PULSE_PIN, HIGH);
 delay(100);
 digitalWrite(PULSE_PIN, LOW);
 delay(100);
 digitalWrite(PULSE_PIN, HIGH);
 delay(100);
 pixels.setBrightness(20); //set Brightness
 pixels.begin();
 pixels.show();
 pinMode      (STROBE_PIN, OUTPUT);
 pinMode      (RESET_PIN,  OUTPUT);  
 digitalWrite (RESET_PIN,  LOW);
 digitalWrite (STROBE_PIN, LOW);
 delay        (1);  
 digitalWrite (RESET_PIN,  HIGH);
 delay        (1);
 digitalWrite (RESET_PIN,  LOW);
 digitalWrite (STROBE_PIN, HIGH);
 delay        (1);
 }

void loop() 
  {    
  counter++;   
  clearspectrum(); 
  if (millis() - pwmpulse > 3000){
  toggle = !toggle;
  digitalWrite(PULSE_PIN, toggle);
  pwmpulse = millis();
  }
  digitalWrite(RESET_PIN, HIGH);
  delayMicroseconds(3000);
  digitalWrite(RESET_PIN, LOW);
  for(int i=0; i < COLUMNS; i++){ 
    
  digitalWrite(STROBE_PIN, LOW);
  delayMicroseconds(1000);
  spectrumValue[i] = analogRead(0);
  if(spectrumValue[i] < 120)spectrumValue[i] = 0;
  spectrumValue[i] = constrain(spectrumValue[i], 0, 1023);
  spectrumValue[i] = map(spectrumValue[i], 0, 1023, 0, ROWS);i++;
  spectrumValue[i] = analogRead(1);
  if(spectrumValue[i] < 120)spectrumValue[i] = 0;
  spectrumValue[i] = constrain(spectrumValue[i], 0, 1023);
  spectrumValue[i] = map(spectrumValue[i], 0, 1023, 0, ROWS);
  digitalWrite(STROBE_PIN, HIGH);  
  }
  for(int j = 0; j < COLUMNS; j++){
  for(int i = 0; i < spectrumValue[j]; i++){ 
  spectrum[i][COLUMNS - 1 - j].active = 1;
  spectrum[i][COLUMNS - 1 - j].r =0;           //COLUMN Color red
  spectrum[i][COLUMNS - 1 - j].g =255;         //COLUMN Color green
  spectrum[i][COLUMNS - 1 - j].b =0;           //COLUMN Color blue
  }
  if(spectrumValue[j] - 1 > peakhold[j].position)
  {
  spectrum[spectrumValue[j] - 1][COLUMNS - 1 - j].r = 0; 
  spectrum[spectrumValue[j] - 1][COLUMNS - 1 - j].g = 0; 
  spectrum[spectrumValue[j] - 1][COLUMNS - 1 - j].b = 0;
  peakhold[j].position = spectrumValue[j] - 1;
  peakhold[j].peakpause = 1; //set peakpause
  }
  else
  {
  spectrum[peakhold[j].position][COLUMNS - 1 - j].active = 1;
  spectrum[peakhold[j].position][COLUMNS - 1 - j].r = 255;  //Peak Color red
  spectrum[peakhold[j].position][COLUMNS - 1 - j].g = 255;  //Peak Color green
  spectrum[peakhold[j].position][COLUMNS - 1 - j].b = 0;    //Peak Color blue
  }
  } 
  flushMatrix();
  if(counter % 3 ==0)topSinking(); //peak delay
  }
  void topSinking()
  {
  for(int j = 0; j < ROWS; j++)
  {
  if(peakhold[j].position > 0 && peakhold[j].peakpause <= 0) peakhold[j].position--;
  else if(peakhold[j].peakpause > 0) peakhold[j].peakpause--;       
  } 
  }
  void clearspectrum()
  {
  for(int i = 0; i < ROWS; i++)
  {
  for(int j = 0; j < COLUMNS; j++)
  {
  spectrum[i][j].active = false;  
  } 
  }
  }
  void flushMatrix()
  {
  for(int j = 0; j < COLUMNS; j++)
  {
  if( j % 2 != 0)
  {
  for(int i = 0; i < ROWS; i++)
  {
  if(spectrum[ROWS - 1 - i][j].active)
  {
  pixels.setPixelColor(j * ROWS + i, pixels.Color(
  spectrum[ROWS - 1 - i][j].r, 
  spectrum[ROWS - 1 - i][j].g, 
  spectrum[ROWS - 1 - i][j].b));         
  }
  else
  {
  pixels.setPixelColor( j * ROWS + i, 0, 0, 0);  
  } 
  }
  }
  else
  {
  for(int i = 0; i < ROWS; i++)
  {
  if(spectrum[i][j].active)
  {
  pixels.setPixelColor(j * ROWS + i, pixels.Color(
  spectrum[i][j].r, 
  spectrum[i][j].g, 
  spectrum[i][j].b));     
  }
  else
  {
  pixels.setPixelColor( j * ROWS + i, 0, 0, 0);  
  }
  }      
  } 
  }
  pixels.show();
  }
  
