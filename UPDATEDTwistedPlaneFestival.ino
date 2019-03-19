#include <FastLED.h>

// Side A
#define LED_PIN1    10
#define LED_PIN2    8
#define LED_PIN3    6
#define LED_PIN4    4
//Side B
#define LED_PIN5    18
#define LED_PIN6    16
#define LED_PIN7    14
#define LED_PIN8    12

#define NUM_LEDS     50     // 50 per strip
#define NUM_STRIPS   28     // 2 x 14 strips per side
#define SIDE_STRIPS  14     // 14 strips per side
#define TOTAL_LEDS   1400   // So many LEDs...

#define NUM_LEDS3    150    // 150 per 3 strips
#define NUM_LEDS4    200    // 200 per 4 strips

#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB

#define FRAMES_PER_SECOND 30
#define SESSION_TIME 600000 // 1000 = 1 second, 60,000 = 1 minute. 600,000 = 10 minutes. 3,600,000 = 1 hour. 
//#define THRESHOLD 30000
#define FADETIME 3000
#define FADETIMESEC 2 // Pulse

CRGBArray<NUM_LEDS3> batch1;
CRGBArray<NUM_LEDS3> batch2;
CRGBArray<NUM_LEDS4> batch3;
CRGBArray<NUM_LEDS4> batch4;

CRGBArray<NUM_LEDS3> batch5;
CRGBArray<NUM_LEDS3> batch6;
CRGBArray<NUM_LEDS4> batch7;
CRGBArray<NUM_LEDS4> batch8;

CRGBSet ledStrip[NUM_STRIPS] = {
  CRGBSet(batch1(0,49)),
  CRGBSet(batch1(99,50)),
  CRGBSet(batch1(100,149)),
 
  CRGBSet(batch2(0,49)),
  CRGBSet(batch2(99,50)),
  CRGBSet(batch2(100,149)),
 
  CRGBSet(batch3(0,49)),
  CRGBSet(batch3(99,50)),
  CRGBSet(batch3(100,149)),
  CRGBSet(batch3(199,150)),
 
  CRGBSet(batch4(0,49)),
  CRGBSet(batch4(99,50)),
  CRGBSet(batch4(100,149)),
  CRGBSet(batch4(199,150)),

  CRGBSet(batch5(0,49)),
  CRGBSet(batch5(99,50)),
  CRGBSet(batch5(100,149)),
 
  CRGBSet(batch6(0,49)),
  CRGBSet(batch6(99,50)),
  CRGBSet(batch6(100,149)),
 
  CRGBSet(batch7(0,49)),
  CRGBSet(batch7(99,50)),
  CRGBSet(batch7(100,149)),
  CRGBSet(batch7(199,150)),
 
  CRGBSet(batch8(0,49)),
  CRGBSet(batch8(99,50)),
  CRGBSet(batch8(100,149)),
  CRGBSet(batch8(199,150)),
};

// GLOBAL VARIABLES
int secondHand;
unsigned long currentTime;
unsigned long relCurrentTime;
unsigned long previousTime;
unsigned long threshold;
bool firstRun;
bool firstFade;
bool transitionRun;

byte ledArray[NUM_STRIPS][NUM_LEDS];
byte stripArray[NUM_LEDS][3]; //HSV/RGB Colors

//fire
uint8_t hue;

//pulse
uint8_t brightVal[NUM_STRIPS];
bool reverse;

// Temp Palette used
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType    currentBlending;
uint8_t maxChanges; // nblend change rate

// FIRE Definitions
#define COOLING 55
#define SPARKING 120

// PULSE Definitions
#define PULSESPD 3 // def (1)

// ** COLOR PALETTES **
// Color Palette: Rosa BlueWhiteRed palette
extern CRGBPalette16 BlueWhiteRed;
extern const TProgmemPalette16 BlueWhiteRed_p PROGMEM;
// Color Palette: Rosa PurpleBlueAqua palette
extern CRGBPalette16 PurpleBlueAqua;
extern const TProgmemPalette16 PurpleBlueAqua_p PROGMEM;
// Color Palette: Rosa GreenYellowOrange palette
extern CRGBPalette16 GreenYellowOrange;
extern const TProgmemPalette16 GreenYellowOrange_p PROGMEM;
// Color Palette: Rosa YellowOrangePink palette
extern CRGBPalette16 YellowOrangePink;
extern const TProgmemPalette16 YellowOrangePink_p PROGMEM;

// *************************
// ** SETUP **
// *************************

void setup() {
    delay( 1000 ); // power-up safety delay
    // Add LEDs in their batches to program
    FastLED.addLeds<LED_TYPE, LED_PIN1, COLOR_ORDER>(batch1, NUM_LEDS3).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_PIN2, COLOR_ORDER>(batch2, NUM_LEDS3).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_PIN3, COLOR_ORDER>(batch3, NUM_LEDS4).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_PIN4, COLOR_ORDER>(batch4, NUM_LEDS4).setCorrection(TypicalLEDStrip);
    
    FastLED.addLeds<LED_TYPE, LED_PIN5, COLOR_ORDER>(batch5, NUM_LEDS3).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_PIN6, COLOR_ORDER>(batch6, NUM_LEDS3).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_PIN7, COLOR_ORDER>(batch7, NUM_LEDS4).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_PIN8, COLOR_ORDER>(batch8, NUM_LEDS4).setCorrection(TypicalLEDStrip);
    
    FastLED.setBrightness(BRIGHTNESS);

    previousTime = 0;
    relCurrentTime = 0;
    threshold = 1800000;
    firstRun = true;
    firstFade = true;
    transitionRun = false;
    maxChanges = 45;
}

// *************************
// ** MAIN LOOP **
// *************************

void loop() {
    currentTime = millis();
    relCurrentTime = currentTime - previousTime;
    static uint8_t selectedEffect = 0; // 18 = FADE OUT
    static uint8_t tempEffect = 0;

    // Run fade-out program from treshold
    if(relCurrentTime > (threshold - FADETIME)){
        tempEffect = selectedEffect;
        previousTime = currentTime;
        selectedEffect = 18; // Run fade out program
        firstFade = false;
        firstRun = true;
    }
    // Run next program
    if (selectedEffect == 18 && relCurrentTime > FADETIME){
        previousTime = currentTime;
        selectedEffect = tempEffect + 1;
        firstRun = true;
        firstFade = true;
        clearAllNA();
    }

    // Only run on chosen code
    if(selectedEffect > 2 && selectedEffect != 18) {
        selectedEffect = 0;
        firstRun = true;
    }

    switch(selectedEffect) {
        // COLOR PALETTE
        case 0  : {
                    if(firstRun){
                        currentPalette = RainbowColors_p;
                        currentBlending = LINEARBLEND;
                        firstRun = false;
                        threshold = 1800000;
                    }
                    runColorPalette();
                    break;
                  }
        // FIRE PALETTE SWAP
        case 1  : {
                    if(firstRun){
                        resetledArray();
                        currentPalette = HeatColors_p;
                        targetPalette = PurpleBlueAqua_p;
                        hue = 0;
                        firstRun = false;
                        threshold = 1800000;
                    }
                    runFirePrograms();
                    break;
                  }
        // PULSE                  
        case 2  : {
                    if(firstRun){
                        currentPalette = PurpleBlueAqua_p;
                        currentBlending = LINEARBLEND;
                        resetbrightArray();
                        reverse = false;
                        firstRun = false;
                        threshold = 1800000;
                    }
                    runPulseProg();
                    break;
                  }
        // SNOW SPARKLE
        case 5  : {
                    if(firstRun){
                        firstRun = false;
                        threshold = 120000;
                    }
                    runSnowSparkle();
                    
                    break;
                  }
        // STRIPES / COLORWIPE
        case 3  : {
                    if(firstRun){
                        firstRun = false;
                        threshold = 20000;
                    }
                    runStripes();
                    break;
                  }
        // LASER SHOTS
        case 4  : {
                    if(firstRun){
                        resetstripArray();
                        firstRun = false;
                        threshold = 20000;
                    }
                    runLaserShots();
                    break;
                  }
        case 6  : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 7  : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 8  : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 9  : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 10 : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 11 : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 12 : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 13 : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 14 : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 15 : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 16 : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 17 : {
                    if(firstRun){
                        firstRun = false;
                    }
                    break;
                  }
        case 18 : {
                    if(firstRun){
                        firstRun = false;
                    }
                    fadeOut(5);
                    break;
                  }
  }
}

// *************************
// ** FUNCTIONS **
// *************************

// COLOR PALETTE
void runColorPalette() {
    static uint8_t startIndex = 0;
    secondHand = (relCurrentTime / 1000) % (1260);
    if( secondHand == 0)        { currentPalette = RainbowColors_p;      currentBlending = LINEARBLEND; 
                                        targetPalette = OceanColors_p;}
    else if( secondHand == 300)  { currentPalette = OceanColors_p;        currentBlending = LINEARBLEND; 
                                        targetPalette = ForestColors_p;}
    else if( secondHand == 420)  { currentPalette = ForestColors_p;        currentBlending = LINEARBLEND; 
                                        targetPalette = CloudColors_p;}
    else if( secondHand == 540)  { currentPalette = CloudColors_p;         currentBlending = LINEARBLEND; 
                                        targetPalette = HeatColors_p;}
    else if( secondHand == 660)  { currentPalette = HeatColors_p;       currentBlending = LINEARBLEND; 
                                        targetPalette = PartyColors_p;}
    else if( secondHand == 780)  { currentPalette = PartyColors_p;        currentBlending = LINEARBLEND; 
                                        targetPalette = BlueWhiteRed_p;}
    else if( secondHand == 900)  { currentPalette = BlueWhiteRed_p;         currentBlending = LINEARBLEND; 
                                        targetPalette = PurpleBlueAqua_p;}
    else if( secondHand == 1020)  { currentPalette = PurpleBlueAqua_p;    currentBlending = LINEARBLEND; 
                                        targetPalette = GreenYellowOrange_p;}
    else if( secondHand == 1140)  { currentPalette = GreenYellowOrange_p;    currentBlending = LINEARBLEND; 
                                        targetPalette = RainbowColors_p;}
    if( secondHand < 300 && secondHand >= 285){
        nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
    } else if( currentPalette != RainbowColors_p && (secondHand-300)%120 >= 105) {
        nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
    }
    startIndex = startIndex + 2; // speed of wave motion and color transitions
    FillLEDsFromPaletteColors(startIndex);
    FastLED.show();
}
void FillLEDsFromPaletteColors( uint8_t colorIndex ) {
    uint8_t tempIndex1 = colorIndex; // tempIndex1/2 allow all strips to be on same
    uint8_t tempIndex2 = colorIndex;
    uint8_t brightness = 255;
    for(uint8_t j = 0; j < NUM_STRIPS; j++) {
        for( uint8_t i = 0; i < NUM_LEDS; i++) {
            ledStrip[j][i] = ColorFromPalette( currentPalette, tempIndex1, brightness, currentBlending);
            tempIndex1 = tempIndex1 - 2;
            if(i==0) {
              colorIndex = colorIndex - 2;
            }
        }
        tempIndex1 = tempIndex2;
    }
}

// FIRE PALETTE SWAP
void runFirePrograms() {
    secondHand = (relCurrentTime / 1000) % (540);
    if( secondHand < 120) {
        if(!transitionRun) {
            currentPalette = HeatColors_p;
            currentBlending = LINEARBLEND; 
        }
        targetPalette = PurpleBlueAqua_p;
        runFire2012PaletteSwap(false);
    } else if( secondHand < 240) {
        if(!transitionRun) {
            currentPalette = PurpleBlueAqua_p;
            currentBlending = LINEARBLEND;  
        }
        CRGB darkcolor  = CHSV(0,255,192); // pure hue, three-quarters brightness
        CRGB lightcolor = CHSV(0,128,255); // half 'whitened', full brightness
        targetPalette = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);
        runFire2012PaletteSwap(false);
    } else if( secondHand < 540) {
        currentBlending = LINEARBLEND; 
        targetPalette = HeatColors_p;
        runFire2012PaletteSwap(true);
    }
    if( secondHand > 105 && secondHand < 120) {
        transitionRun = true;
        nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
    } else if( secondHand > 225 && secondHand < 240) {
        transitionRun = true;
        nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
    } else if( secondHand > 525 && secondHand < 540) {
        transitionRun = true;
        nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
    }
    if(secondHand == 0 || secondHand == 120 || secondHand == 240 || secondHand == 540){
        transitionRun = false;
    }
}
void runFire2012PaletteSwap(bool hueChange) {
    random16_add_entropy( random()); // FIRE PALETTE SWAP
    if(hueChange && !transitionRun){
        hue++; 
        CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
        CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
        currentPalette = CRGBPalette16(CRGB::Black, darkcolor, lightcolor, CRGB::White); 
    }
    for(uint8_t s = 0;s<NUM_STRIPS/2;s++){
        int coolS = COOLING + (2*s);
        int lossS = 5*s;
        int sparkS = s;
        Fire2012SpecPal(s,coolS,lossS,sparkS);
    }
    FastLED.show();
}
void Fire2012SpecPal(int stripI, int coolI, int heatLoss, int sparkLoss) {
    // Step 1.  Cool down every cell a little
    for( uint8_t i = 0; i < NUM_LEDS; i++) {
        ledArray[stripI][i] = qsub8( ledArray[stripI][i], random8(0,((coolI * 10) / NUM_LEDS) + 2));
    }
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( uint8_t k= NUM_LEDS - 1; k >= 2; k--) {
        ledArray[stripI][k] = (ledArray[stripI][k - 1] + ledArray[stripI][k - 2] + ledArray[stripI][k - 2]) / 3;
    }
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING - sparkLoss) {
        uint8_t y = random8(7);
        ledArray[stripI][y] = qadd8(ledArray[stripI][y], random8(160-heatLoss,255-heatLoss));
    }
    // Step 4.  Map from heat cells to LED colors
    for( uint8_t j = 0; j < NUM_LEDS; j++) {
        // Scale the heat value from 0-255 down to 0-240
        // for best results with color palettes.
        byte colorindex = scale8(ledArray[stripI][j], 240);
        CRGB color = ColorFromPalette(currentPalette, colorindex);
        ledStrip[stripI][j] = color;
        ledStrip[SIDE_STRIPS+stripI][j] = color;
    }
}

// SNOW SPARKLE
void runSnowSparkle(){
    SnowSparkle(10, 15, 10, 20, random16(100,1000));
}
void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
    setAll(red,green,blue);
    uint8_t numSparkles = random8(20);
    for(uint8_t i = 0; i < numSparkles; i++){
        uint8_t pixel = random16(TOTAL_LEDS);
        ledStrip[pixel/NUM_LEDS][pixel%NUM_LEDS] = CRGB::White;
    }
        FastLED.show();
        delay(SparkleDelay);
        setAll(red,green,blue);
        FastLED.show();
        delay(SpeedDelay);
}

// STRIPES
void runStripes(){
    colorWipe(1, 0,255,0, 0);
    colorWipe(1, 0,0,0, 0);
    colorStripe(2,0,0,255,0);
    colorStripe(2,0,0,0,0);
    colorWipe2(3,4, 255,0,0, 0);
    colorWipe2(3,4, 0,0,0, 0);
}
// Lit up one by one LED (should show due to Arduino speed)
void colorWipe(uint8_t strip, byte red, byte green, byte blue, int speedDelay) {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
      ledStrip[strip][i] = CRGB(red,green,blue);
      FastLED.show();
      //delay(speedDelay);
  }
}
// Lit up one by one LED (should show due to Arduino speed)
void colorWipe2(uint8_t strip1, uint8_t strip2, byte red, byte green, byte blue, int speedDelay) {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
      ledStrip[strip1][i] = CRGB(red,green,blue);
      ledStrip[strip2][i] = CRGB(red,green,blue);
      FastLED.show();
      //delay(speedDelay);
  }
}
// Whole strip lit up at once
void colorStripe(uint8_t strip, byte red, byte green, byte blue, int speedDelay) {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
      ledStrip[strip][i] = CRGB(red,green,blue);
      delay(500);
      //delay(speedDelay);
  }
  FastLED.show();
}
void colorStripe2(uint8_t strip1, uint8_t strip2, byte red, byte green, byte blue, int speedDelay) {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
      ledStrip[strip1][i] = CRGB(red,green,blue);
      ledStrip[strip2][i] = CRGB(red,green,blue);
      delay(500);
      //delay(speedDelay);
  }
  FastLED.show();
}

// LASER SHOTS
void runLaserShots(){
    meteorRain(0xff,0xff,0xff,10, 64, true, 30);
}
void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
  for(int i = 0; i < NUM_LEDS+NUM_LEDS; i++) {
    // METEOR1
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        ledStrip[1][j].fadeToBlackBy(meteorTrailDecay );        
      }
    }
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
        ledStrip[17][i-j] = CRGB(red,green,blue);
      } 
    }
        // METEOR2
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        ledStrip[1][j].fadeToBlackBy(meteorTrailDecay );        
      }
    }
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
        ledStrip[17][i-j] = CRGB(red,green,blue);
      } 
    }
  }
}

// PULSE
void runPulseProg() {
  runPulse();
}
void runPulse() {
    secondHand = (relCurrentTime / 1000) % 600;
    if( secondHand < 120 ){
        targetPalette = YellowOrangePink_p;
        pulse();
    } else if( secondHand < 240 ){
        targetPalette = GreenYellowOrange_p;
        pulse();
    } else if( secondHand < 360 ){
        targetPalette = OceanColors_p;
        pulse();
    } else if( secondHand < 480 ){
        targetPalette = ForestColors_p;
        pulse();
    } else if( secondHand < 600 ){
        targetPalette = PurpleBlueAqua_p;
        pulse();
    }
    if (secondHand%120 > 105) {
        nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
    }
    FastLED.show();
}
void pulse(){
    for( int j = 0; j < NUM_STRIPS; j++ ) {
        for( int i = 0; i < NUM_LEDS; i++) {
            ledStrip[j][i] = ColorFromPalette( currentPalette, 2*j, brightVal[j], currentBlending);
        }
        if(brightVal[j] > 180) {
            reverse = true;
        } else if (brightVal[j] < 50) {
            reverse = false;
        }
        if(reverse){
            brightVal[j] = brightVal[j] - PULSESPD;
        } else {
            brightVal[j] = brightVal[j] + PULSESPD;
        }
    }
}
void changePulsePal(uint8_t strip, CRGBPalette16 targetPalette){
    if(brightVal[strip] == 0) {
        currentPalette = targetPalette;
    }
    else {
        brightVal[strip] = brightVal[strip] - PULSESPD;
    }
}

// *************************
// ** CUSTOM PALETTES **
// *************************

// Rosa Custom Palette BlueWhiteRed COLOR PALETTE
const TProgmemPalette16 BlueWhiteRed_p PROGMEM = {
    CRGB::Aqua,
    CRGB::Amethyst,
    CRGB::Coral,
    CRGB::MistyRose,
    
    CRGB::Plum,
    CRGB::PowderBlue,
    CRGB::SkyBlue,
    CRGB::Turquoise,
    
    CRGB::Coral,
    CRGB::MistyRose,
    CRGB::Seashell,
    CRGB::Sienna,
    
    CRGB::Aqua,
    CRGB::Amethyst,
    CRGB::Coral,
    CRGB::MistyRose,
};

// Rosa Custom Palette PurpleBlueAqua COLOR PALETTE
const TProgmemPalette16 PurpleBlueAqua_p PROGMEM = {
    CRGB::Aqua,
    CRGB::Amethyst,
    CRGB::Coral,
    CRGB::MistyRose,
    
    CRGB::Plum,
    CRGB::PowderBlue,
    CRGB::SkyBlue,
    CRGB::Turquoise,
    
    CRGB::Coral,
    CRGB::MistyRose,
    CRGB::Seashell,
    CRGB::Sienna,
    
    CRGB::Aqua,
    CRGB::Amethyst,
    CRGB::Coral,
    CRGB::MistyRose,
};

// Rosa Custom Palette GreenYellowOrange COLOR PALETTE
const TProgmemPalette16 GreenYellowOrange_p PROGMEM = {
    CRGB::Aqua,
    CRGB::Amethyst,
    CRGB::Coral,
    CRGB::MistyRose,
    
    CRGB::Plum,
    CRGB::PowderBlue,
    CRGB::SkyBlue,
    CRGB::Turquoise,
    
    CRGB::Coral,
    CRGB::MistyRose,
    CRGB::Seashell,
    CRGB::Sienna,
    
    CRGB::Aqua,
    CRGB::Amethyst,
    CRGB::Coral,
    CRGB::MistyRose,
};

// Rosa Custom Palette YellowOrangePink COLOR PALETTE
const TProgmemPalette16 YellowOrangePink_p PROGMEM = {
    CRGB::Gold,
    CRGB::Gold,
    CRGB::Orange,
    CRGB::Orange,
    
    CRGB::OrangeRed,
    CRGB::OrangeRed,
    CRGB::HotPink,
    CRGB::HotPink,
    
    CRGB::Pink,
    CRGB::Pink,
    CRGB::OrangeRed,
    CRGB::OrangeRed,
    
    CRGB::Orange,
    CRGB::Orange,
    CRGB::Gold,
    CRGB::Gold,
};


// *************************
// ** COMMON FUNCTIONS **
// *************************

void fadeOut(int sec) {
    for(uint8_t j = 0; j < NUM_STRIPS; j++){
        for(uint8_t i = 0; i < NUM_LEDS; i++){
            ledStrip[j][i].fadeToBlackBy( 15 );
        }
    }
    FastLED.show();
}

// Clear All functions
// Set all LEDs to OFF and apply it (visible)
void clearAll() {
    for(uint8_t j = 0; j < NUM_STRIPS; j++){
        for(uint8_t i = 0; i < NUM_LEDS; i++ ) {
            ledStrip[j][i] = CRGB::Black;
        }
    }
    FastLED.show();
}
// Set all LEDs to OFF but don't apply it
void clearAllNA() {
    for(uint8_t j = 0; j < NUM_STRIPS; j++){
        for(uint8_t i = 0; i < NUM_LEDS; i++ ) {
            ledStrip[j][i] = CRGB::Black;
        }
    }
}

// Set All functions
// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue) {
    for(uint8_t j = 0; j < NUM_STRIPS; j++){
        for(uint8_t i = 0; i < NUM_LEDS; i++ ) {
            ledStrip[j][i] = CRGB(red,green,blue);
        }
    }
    FastLED.show();
}
// Set all LEDs to a given color but don't apply it
void setAllNA(byte red, byte green, byte blue) {
    for(uint8_t j = 0; j < NUM_STRIPS; j++){
        for(uint8_t i = 0; i < NUM_LEDS; i++ ) {
            ledStrip[j][i] = CRGB(red,green,blue);
        }
    }
}

// Array resets
void resetledArray() {
    for(uint8_t j = 0; j < NUM_STRIPS; j++){
        for(uint8_t i = 0; i < NUM_LEDS; i++ ) {
            ledArray[j][i] = 0;
        }
    }
}
void resetstripArray() {
    for(uint8_t j = 0; j < NUM_LEDS; j++ ) {
        for(uint8_t i = 0; i < 3; i++) {
            stripArray[j][i] = 0;
        }
    }
}
void resetbrightArray() {
    for(uint8_t i = 0; i < NUM_STRIPS; i++){
        brightVal[i] = 0;
    }
}
