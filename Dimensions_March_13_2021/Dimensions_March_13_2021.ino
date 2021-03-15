#define SCLK_PIN 18  
#define MOSI_PIN 23
#define DC_PIN   17
#define CS_PIN   5
#define RST_PIN  4

// DIMENSIONS
// Erik Oostveen (www.erikoostveen.co.uk)
// Version 1.0
// March 2021

// Color definitions (RGB565)
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF
#define GREY            0x4208
#define ORANGE          0xFD20
#define PINK            0xF8FF
#define DARKGREY        0x18c3
#define LIGHTBLUE       0x06df
#define PURPLE          0x5996

#include <esp32-hal-cpu.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <MIDI.h>
#include <Button.h>
#include <EEPROM.h>
#include <Fonts/FreeSans9pt7b.h>

int cmpfunc (const void * aaa, const void * bbb) { return ( *(int*)aaa - *(int*)bbb ); } // declaration for qsort() function.

// Set total number of Parametric Equations
int numberOfParametricEQs = 20;
float numberOfParametricEQs_fl = 20.0;

// Set MIDI out channel

  int xCube = 59;
  int yCube = 40;
  int aCube = 20;
  int bCube = 10;
  int pointShift;

  int midiTimeOut = 0;
  int midiTimeOutBlankScreen = 0;

#define EEPROM_SIZE 198

MIDI_CREATE_DEFAULT_INSTANCE();
int midiClockCounter = 0;

int operationMode = 0; // 0 = Normal, 1 = Ena/Dis Notes, 2 = Set Intervals

float gridSelection_fl = 0.0;
int gridSelection = 0;
int gridSelectionBox_X;
int midiNotesArr[] = {108,109,110,111,112,113,114,115,116,117,118,119,96,97,98,99,100,101,102,103,104,105,106,107,84,85,86,87,88,89,90,91,92,93,94,95,72,72,74,75,76,77,78,79,80,81,82,83,60,61,62,63,64,65,66,67,68,69,70,71,48,49,50,51,52,53,54,55,56,57,58,59,36,37,38,39,40,41,42,43,44,45,46,47,24,25,26,27,28,29,30,31,32,33,34,35,12,13,14,15,16,17,18,19,20,21,22,23};
String midiNotesTXTarr[108] = {"C0","C0#","D0","D0#","E0","F0","F0#","G0","G0#","A0","A0#","B0","C1","C1#","D1","D1#","E1","F1","F1#","G1","G1#","A1","A1#","B1","C2","C2#","D2","D2#","E2","F2","F2#","G2","G2#","A2","A2#","B2","C3","C3#","D3","D3#","E3","F3","F0#","G3","G3#","A3","A3#","B3","C4","C4#","D4","D4#","E4","F4","F4#","G4","G4#","A4","A4#","B4","C5","C5#","D5","D5#","E5","F5","F5#","G5","G5#","A5","A5#","B5","C6","C6#","D6","D6#","E6","F6","F6#","G6","G6#","A6","A6#","B6","C7","C7#","D7","D7#","E7","F7","F7#","G7","G7#","A7","A7#","B7","C8","C8#","D8","D8#","E8","F8","F8#","G8","G8#","A8","A8#","B8"};

int EnaDisNotesArr[108];
int midiSubSelectionNotesArr[108];
int midiSubSelectionNotesPointer;
int notesShuffled = 0;
int snapShotRetrieved = 0;

int snapShotRetrievedInterval = 0;

int SNAPSHOT_1_midiSubSelectionNotesArr[108] = {45}; // INIT Snapshot :: Note # 45 = A2
int SNAPSHOT_2_midiSubSelectionNotesArr[108] = {48}; // INIT Snapshot :: Note # 48 = C3

float gridSelectionInterval_fl = 0.0;
int gridSelectionInterval = 0;
int gridSelectionBoxInterval_X;

int EnaDisIntervalArr[90];
int midiSubSelectionIntervalArr[10];
int midiSubSelectionIntervalPointer;
int intervalDiv;
int newInterval;
int gridNumber;

int SNAPSHOT_1_midiSubSelectionIntervalArr[10] = {12,12,12,12,12,12,12,12,12,12}; // INIT Snapshot :: Note # 45, A2 at 1/8
int SNAPSHOT_2_midiSubSelectionIntervalArr[10] = {12,12,12,12,12,12,12,12,12,12}; // INIT Snapshot :: Note # 48, A2 at 1/8

int retrievedPattern;
int midiSubSelectionNotesPointerSnap_1;
int midiSubSelectionNotesPointerSnap_2;

int clkDivDefaultsArr[] =            {3,    4,     6,      8,    12,   16,   24,  32,     48,   72,  96};
String clkDivDefaultsTXTarr[11] = {"1/32","1/24","1/16","1/12","1/8","1/6","1/4","1/3" ,"1/2","3/4","1"};

//int IntervalNoteMillisArr[] = {90,135,180,270,360,540,720,1080,1440}; // "Sustain" milliseconds based on selected interval (1st attempt: sound a bit long ... so, see below, using 1/2 times instead)
int IntervalNoteMillisArr[] = {45,67,90,135,180,270,360,540,720}; // "Sustain" milliseconds based on selected interval
//                             1/16,1/12,1/8,1/6,1/4,1/3,1/2,3/4,1
//int IntervalNoteMillisArr[] = {22,33,45,67,90,135,360,540,800}; // "Sustain" milliseconds based on selected interval

int IntervalNoteMillis;

int IntervalNoteMillisOutputArr[9];

int stepsPointer = 4; // Default is 32Steps or 192 MIDI clock bytes
float stepsPointer_fl = 3.0; 
int stepsDefaults[] = {11,23,47,95,191,383,767,3071};
String stepsDefaultsTXTarr[9] = {"2 steps","4 steps","8 steps","16 steps","32 steps","64 steps","128 steps","256 steps"};

int clkDiv = 12;

float velocityMode_fl = 0.0;
int velocityMode = 0;

float thruMode_fl = 0.0;
int thruMode = 0;

int playCircleArr[] = {3, 6, 12, 24, 48, 96, 192, 767}; // 384
int playCircle;

float sustainNoteMode_fl = 0.0;
int sustainNoteMode = 0;

Adafruit_SSD1351 tft = Adafruit_SSD1351(128, 128, &SPI, CS_PIN, DC_PIN, RST_PIN);

// Some 1.5" OLED 128x128 Diplay may have some problems keeping up with the SPI frequency of 16mHz
// In the Adafruit_SSD1351.cpp (of the Adafruit_SSD1351_library) change:
// #define SPI_DEFAULT_FREQ 16000000 ///< ESP32 SPI default frequency  
// to:
// #define SPI_DEFAULT_FREQ 10000000 ///< ESP32 SPI default frequency   

// Param ena/dis -----------------
int param_a = 1;
int param_b = 1;
int param_c = 1;
int param_d = 1;
int param_rnd = 1;
// -------------------------------

// Encoder -----------------------
#define Enc_CLK 19
#define Enc_DT 21
int Enc_currentStateCLK;
int Enc_previousStateCLK; 
Button encoderSwitch(22);
// Encoder -----------------------

// Parameter switches ------------
Button parameterUP(34);
Button showParam(35);
Button parameterDOWN(32);
// Parameter switches ------------

// Mode switches ------------
#define modeSwitchAPin 25
#define modeSwitchBPin 26
int clearScreenBetweenSwitches = 0;
// Mode switches ------------

// Note switch ------------
#define noteSwitchPin 27
int NoteEnaDis = 1;
int NoteEnaDisNoteOff = 1;
// Note switch ------------

// MIDI Start/Stop switch ------------
#define midiStartStopSwitchPin 14
int midiStartStop = 1; // 1 = MIDI Start/Stop only, 0 = Free Run & MIDI Start
int DimensionsGo = 0;
// Note switch -----------------------

// Snapshot switches -----------------
Button takeSnapshot_1_Pin(15);
Button takeSnapshot_2_Pin(13);
Button retrieveSnapshot_1_Pin(16); 
Button retrieveSnapshot_2_Pin(5);
// -----------------------------------

// Duration Pot ------------------
int potAvalue;
// Duration Pot ------------------

// Parameters ============================================================
int displayCounter = 0; // x number of loops before clearing displayed parameter changes

float t = 50;           // t = time (sync with MIDI clock)

float a_fl = 10.0 ;
int a = 10;        
float b_fl = 10.0 ;
int b = 10;        
float c_fl = 1.0 ; 
int c = 1;  
float d_fl = 1.0 ;
int d = 1;        
float rnd_fl = 1.0 ; 
int rnd = 0;  

float fx_select_fl = 1.0;
int fx_select = 1;

float pz_offset_fl = 127.0;
int pz_offset = 127;

float trace_fl = 0.0;
int trace = 0;

float triggerMode_fl = 0.0;
int triggerMode = 0;

float value_gran = 0.0125; // value_gran = delta_gran_fl / 2000
float delta_gran_fl = 25.0;
int delta_gran = 25;

float midi_channel_fl = 1.0;
int midi_channel = 1;

int updown = 1; // 1 = count UP, 0 = count DOWN

float x_fl = 64.0;
int x=64;

float y_fl = 64.0;
int y=64;

float value_t1_fl = 50.0;
int value_t1 = 50;

float value_t2_fl = 64.0;
int value_t2 = 64; 

double px_fl;
int px;

double py_fl;
int py;

double pz_fl;
int pz;

float pyRest_fl = 0.0;
int pyRest = 0;

float octTranspose_fl = 0.0; 
int octTranspose = 0;

int toNormal = 0;

// Define, draw and manage tail =============
int taillength = 10;
int tt = 10; 
int tailpx_list[10];
int tailpy_list[10];
int tailpz_list[10];
// ==========================================
int pz_draw;

int parameterSelection = 0; // 0 = a, 1 = b, 2 = x(-axis offset), 3 = y(-axis offset)

// Note control
unsigned long previousMillis = 0; 
unsigned long currentMillis;  
int Note_playNote = 0;          // 1 = Yes, play a note
int Note_playNoteTrig;          // 1 = Yes, trigger
int Note_Hist = 0;              // Store Note Played
int Note_Pitch;                 // MIDI Note: 0 ... 127
int Note_Velocity;              // MIDI Velocity 0 ... 127
int Note_Duration;              // Duration in ms
int NoteDurationFlag = 0;       // 1 = note duration is ongoing, 0 = finished 
int fetchNote;

int updownPressed = 0;

float startStopFromMenu_fl = 0.0;
int startStopFromMenu = 0;

void setup(void) {

  setCpuFrequencyMhz(160); // 20, 40, 80, 160 & 240mhz 

  pinMode(RST_PIN, OUTPUT);

  tft.begin();

  encoderSwitch.begin();
  parameterUP.begin();
  showParam.begin();
  parameterDOWN.begin();

  takeSnapshot_1_Pin.begin();
  takeSnapshot_2_Pin.begin();
  retrieveSnapshot_1_Pin.begin(); 
  retrieveSnapshot_2_Pin.begin();

  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOn();

  // You can optionally rotate the display by running the line below.
  // Note that a value of 0 means no rotation, 1 means 90 clockwise,
  // 2 means 180 degrees clockwise, and 3 means 270 degrees clockwise.
  //tft.setRotation(1);
  // NOTE: The test pattern at the start will NOT be rotated!  The code
  // for rendering the test pattern talks directly to the display and
  // ignores any rotation.

  // Encoder ---------------------------
  pinMode (Enc_CLK,INPUT);
  pinMode (Enc_DT,INPUT);
  Enc_previousStateCLK = digitalRead(Enc_CLK);
  // Encoder ---------------------------

  // Mode switches ---------------------
  pinMode(modeSwitchAPin, INPUT);
  pinMode(modeSwitchBPin, INPUT);
  // Mode switches ---------------------

  // Note ena/dis switch ---------------------
  pinMode(noteSwitchPin, INPUT);
  // Note ena/dis switch ---------------------

  // MIDI Start/Stop switch ------------
  pinMode(midiStartStopSwitchPin, INPUT);
  // MIDI Start/Stop switch ------------

  clrScreen();
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  //tft.setFont(&FreeSans9pt7b); 

  EEPROM.begin(EEPROM_SIZE);

    // read flash memory and make '0' if read address is not '0' or '1' (possible at very first power up)
    for( int i = 0; i < 108;  ++i ) {
      if ( EEPROM.read(i) > 1 ) {
        EEPROM.write(i, 0);
        EEPROM.commit();                
      }
    EnaDisNotesArr[i] = EEPROM.read(i);     
    }
    drawGridSquaresNotes(); // this is to populate "midiSubSelectionNotesPointer" and "midiSubSelectionNotesArr" so we can start transmitting notes straight away in mode = 0 

    // read flash memory and make '0' if read address is not '0' or '1' (possible at very first power up)
    for( int i = 108; i < 198;  ++i ) {
      if ( EEPROM.read(i) > 1 ) {
        EEPROM.write(i, 0);
        EEPROM.commit();                
      }
    EnaDisIntervalArr[i-108] = EEPROM.read(i);
    }
      // Before we carry on, we need to make sure that the sum of EnaDisIntervalArr = 10. If sum is less or more then it means that
      // some intervals are either not selected or double selected, which shouldn't be possible.
      int sum = 0;
      for ( int i = 0; i < 90; i++ ) {
      sum = sum + EnaDisIntervalArr[i];
      }

      // ... if the sum is not 10, then create the default interval setting (all 1/16th)
      if ( sum != 10 ) {    

        // Populate properly (just one row - 8th only in this case)    
        for ( int i = 108; i < 198; i++ ) {

            if ( i >= 128 && i <= 137 ) {
              EEPROM.write(i, 1);                        
            }
            else {
              EEPROM.write(i, 0);                           
            }

        EEPROM.commit();  
        EnaDisIntervalArr[i-108] = EEPROM.read(i); 
        }
            
      }
      drawGridSquaresInterval();


pinMode(2, OUTPUT);
pinMode(12, OUTPUT);


drawCube();

}


void loop() {

if ( midiTimeOut > 100000 ) { showMidiClockmessage(); midiTimeOut = 100001; } else { midiTimeOut++; }
                                                                                                                  

// Check mode ----------------------------------------------------------------------------------------------------
if ( digitalRead(modeSwitchAPin) == HIGH && digitalRead(modeSwitchBPin) == HIGH ) { // 0 = Normal
    if ( operationMode == 1 || operationMode == 2 ) {
     operationMode = 0;
     clrScreen();
     toNormal = 1;
    }
  }
  
if ( digitalRead(modeSwitchAPin) == LOW && digitalRead(modeSwitchBPin) == HIGH ) { // 1 = Set Notes
    if ( operationMode == 0 || operationMode == 2 ) {
     operationMode = 1; 
     clrScreen();
     gridSelection_fl = 0.0;
     gridSelection = 0;
     displayCounter = 0;
     drawNotesGrid();
    }   
  }

if ( digitalRead(modeSwitchAPin) == HIGH && digitalRead(modeSwitchBPin) == LOW ) { // 2 = Set Intervals
    if ( operationMode == 0 || operationMode == 1 ) {
     operationMode = 2;
     clrScreen();
     gridSelectionInterval_fl = 0.0;
     gridSelectionInterval = 0;     
     displayCounter = 0;
     drawIntervalGrid();
    } 
  }
// --------------------------------------------------------------------------------------------------------------

// Check note ena/dis -------------------------------------------------------------------------------------------
if ( digitalRead(noteSwitchPin) == LOW ) {
  NoteEnaDis = 0;
}
else {
  NoteEnaDis = 1;
}
// Check note ena/dis -------------------------------------------------------------------------------------------

// Check MIDI Start/Stop -------------------------------------------------------------------------------------------
if ( digitalRead(midiStartStopSwitchPin) == LOW ) {
  midiStartStop = 0;
}
else {
  midiStartStop = 1;
}

// Check MIDI Start/Stop -------------------------------------------------------------------------------------------


// Check Take/Retrieve Snapshot switches ---------------------------------------------------------------------------
if (takeSnapshot_1_Pin.pressed()) {

                        memcpy(SNAPSHOT_1_midiSubSelectionNotesArr, midiSubSelectionNotesArr, sizeof midiSubSelectionNotesArr);
                        memcpy(SNAPSHOT_1_midiSubSelectionIntervalArr, midiSubSelectionIntervalArr, sizeof midiSubSelectionIntervalArr);                 
                        
}

if (takeSnapshot_2_Pin.pressed()) {

                        memcpy(SNAPSHOT_2_midiSubSelectionNotesArr, midiSubSelectionNotesArr, sizeof midiSubSelectionNotesArr);
                        memcpy(SNAPSHOT_2_midiSubSelectionIntervalArr, midiSubSelectionIntervalArr, sizeof midiSubSelectionIntervalArr);
                           
}

if (retrieveSnapshot_1_Pin.pressed()) {

                        digitalWrite(2, LOW);
                        digitalWrite(12, HIGH);

                        memcpy(midiSubSelectionNotesArr,SNAPSHOT_1_midiSubSelectionNotesArr, sizeof SNAPSHOT_1_midiSubSelectionNotesArr);

                        retrievedPattern = 1;
                        midiSubSelectionNotesPointerSnap_1 = 0;
                        for( int c = 0; c < 108;  c++ ) {

                          if ( SNAPSHOT_1_midiSubSelectionNotesArr[c] == 0 ) { break; } else { midiSubSelectionNotesPointerSnap_1++; }
                                                      
                        }

                        snapShotRetrieved = 1;
                        getActualNumberOfNotes();

                          memcpy(midiSubSelectionIntervalArr,SNAPSHOT_1_midiSubSelectionIntervalArr, sizeof SNAPSHOT_1_midiSubSelectionIntervalArr);
                                                              
}

if (retrieveSnapshot_2_Pin.pressed()) {

                        digitalWrite(12, LOW);
                        digitalWrite(2, HIGH);
  
                        memcpy(midiSubSelectionNotesArr,SNAPSHOT_2_midiSubSelectionNotesArr, sizeof SNAPSHOT_2_midiSubSelectionNotesArr);

                        retrievedPattern = 2;
                        midiSubSelectionNotesPointerSnap_2 = 0;
                        for( int c = 0; c < 108;  c++ ) {

                          if ( SNAPSHOT_2_midiSubSelectionNotesArr[c] == 0 ) { break; } else { midiSubSelectionNotesPointerSnap_2++; }

                        }

                        snapShotRetrieved = 1;
                        getActualNumberOfNotes();               

                          memcpy(midiSubSelectionIntervalArr,SNAPSHOT_2_midiSubSelectionIntervalArr, sizeof SNAPSHOT_2_midiSubSelectionIntervalArr);
                     
}
// Check Take/Retrieve Snapshot switches ---------------------------------------------------------------------------


// Clear top text after encoder change START ==================================================
if ( displayCounter > 0 ) { displayCounter++; }
if ( displayCounter > 150000 ) { clrTopText(); displayCounter = 0; }
// Clear top text after encoder change END ====================================================
  

  // Check Value of Pot A (Note Duration)
  potAvalue = map(analogRead(33),0,4095,0,750);
  
    if ( (NoteEnaDis == 0 || midiSubSelectionNotesPointer == 0 || ( midiStartStop == 1 && DimensionsGo == 0 ) ) && NoteEnaDisNoteOff == 1 ) { MIDI.sendNoteOff(Note_Hist, 0, midi_channel); delay(2); NoteEnaDisNoteOff = 0; } // When disable notes out, clear last note with note off (this avoids hanging notes)


    if ( Note_playNote == 1 && NoteEnaDis == 1 && midiSubSelectionNotesPointer != 0 && ( ( midiStartStop == 1 && DimensionsGo == 1 ) || midiStartStop == 0 ) ) {

      NoteEnaDisNoteOff = 1;
    
          if ( Note_playNoteTrig == 1 ) {
    
            if ( (Note_Pitch != Note_Hist) && NoteDurationFlag == 1 ) { 
              MIDI.sendNoteOff(Note_Hist, 0, midi_channel);  // Send Note Off in case a new different note comes in before the end of the previous set duration (i.e. "kill" previous note).
              delay(2);                           // PS: this bit of code will send a note off to note 0 the very first time a play note is generated but this is ok.
              // Note_Duration = 0;
              } 
                                                                                                     
           
            if ( Note_Pitch != 0 ) { MIDI.sendNoteOn(Note_Pitch, Note_Velocity, midi_channel); }
            Note_Hist = Note_Pitch; 
            Note_playNoteTrig = 0;
            currentMillis = millis(); 
            previousMillis = currentMillis;
            NoteDurationFlag = 1;
            
          }
    
          currentMillis = millis();  
          if (currentMillis - previousMillis >= Note_Duration) {        
            MIDI.sendNoteOff(Note_Pitch, 0, midi_channel);
            //Note_Duration = 0;
            Note_playNote = 0;
            NoteDurationFlag = 0;
          }
    
    }


                // Read Menu Switches START ================================================================================
                if ( operationMode == 0 ) {

                        if (parameterUP.pressed()) {
                    
                        parameterSelection = parameterSelection + 1;
                    
                          if ( parameterSelection > 21 ) { parameterSelection = 0; }
                          
                        updownPressed = 1;  
                        }
                    
                        if (parameterDOWN.pressed()) {
                    
                        parameterSelection = parameterSelection - 1;
                    
                          if ( parameterSelection < 0 ) { parameterSelection = 21; }
        
                        updownPressed = 1;
                        }
        
                        if ( showParam.pressed() || updownPressed == 1 || toNormal == 1 ) {                         
                          
                          switch (parameterSelection) {
                            case 0:
                              if ( param_a == 0 ) { tft.setTextColor(PINK); } writeParam(a,100); tft.setTextColor(WHITE); 
                              tft.setCursor(0,1); if ( param_a == 0 ) { tft.setTextColor(PINK); } tft.print("1. a f(x) val:"); tft.setTextColor(WHITE);
                              break;
                            case 1:
                              if ( param_b == 0 ) { tft.setTextColor(PINK); } writeParam(b,100); tft.setTextColor(WHITE); 
                              tft.setCursor(0,1); if ( param_b == 0 ) { tft.setTextColor(PINK); } tft.print("2. b f(x) val:"); tft.setTextColor(WHITE); 
                              break;
                            case 2:
                              if ( param_c == 0 ) { tft.setTextColor(PINK); } writeParam(c,100); tft.setTextColor(WHITE);
                              tft.setCursor(0,1); if ( param_c == 0 ) { tft.setTextColor(PINK); } tft.print("3. c f(x) val:"); tft.setTextColor(WHITE);
                              break; 
                            case 3:
                              if ( param_d == 0 ) { tft.setTextColor(PINK); } writeParam(d,100); tft.setTextColor(WHITE); 
                              tft.setCursor(0,1); if ( param_d == 0 ) { tft.setTextColor(PINK); } tft.print("4. d f(x) val:"); tft.setTextColor(WHITE); 
                              break;
                            case 4:
                              if ( param_rnd == 0 ) { tft.setTextColor(PINK); } writeParam(rnd,100); tft.setTextColor(WHITE); 
                              tft.setCursor(0,1); if ( param_rnd == 0 ) { tft.setTextColor(PINK); } tft.print("5. rnd f(x) val:"); tft.setTextColor(WHITE); 
                              break;                                                   
                            case 5:
                              writeParam(delta_gran,100); // Call function to write value 
                              tft.setCursor(0,1); tft.print("6. t step size:");  
                              break; 
                            case 6: 
                              writeParam(x,100); // Call function to write value 
                              tft.setCursor(0,1); tft.print("7. x int offst:"); 
                              break;                       
                            case 7: 
                              writeParam(y,105); // Call function to write value 
                              tft.setCursor(0,1); tft.print("8. y ptch offst:"); 
                              break;                              
                            case 8: 
                              writeParam(value_t1,100); // Call function to write value 
                              tft.setCursor(0,1); tft.print("9. t from (min):"); 
                              break;                     
                            case 9: 
                              writeParam(value_t2,100); // Call function to write value 
                              tft.setCursor(0,1); tft.print("10. t to (max):"); 
                              break; 
                            case 10: 
                              writeParam(pyRest,100); // Call function to write value 
                              tft.setCursor(0,1); tft.print("11. set mask:"); 
                              break;
                            case 11: 
                              clrTopText();
                              if ( triggerMode == 1 ) { 
                                tft.setCursor(0,1); tft.print("12. notes ABOVE mask"); 
                              }
                              else {
                                tft.setCursor(0,1); tft.print("12. notes BELOW mask"); 
                              }
                              break;
                            case 12: 
                              clrTopText();
                              if ( thruMode == 0 ) { 
                                tft.setCursor(0,1); tft.print("13. MIDI Thru ON"); 
                              }
                              else {
                                tft.setCursor(0,1); tft.print("13. MIDI Thru OFF"); 
                              }
                              break;
                            case 13: 
                              writeParam(midi_channel,105); // Call function to write value 
                              tft.setCursor(0,1); tft.print("14. MIDI Tx Chnl:");  
                              break;
                            case 14: 
                              clrTopText();
                              if ( startStopFromMenu == -1 ) { 
                                tft.setCursor(0,1); tft.print("15. Start/Stop STOP"); 
                              }
                              if ( startStopFromMenu == 0 ) {
                                tft.setCursor(0,1); tft.print("15. Start/Stop AUTO"); 
                              }
                              if ( startStopFromMenu == 1 ) {
                                tft.setCursor(0,1); tft.print("15. Start/Stop START"); 
                              }
                              break;
                            case 15: 
                              writeParam(octTranspose,100); // Call function to write value 
                              tft.setCursor(0,1); tft.print("16. transpose:");
                              break;
                            case 16: 
                              clrTopText();
                              if ( sustainNoteMode == 0 ) { 
                                tft.setCursor(0,1); tft.print("17. sustain note OFF"); 
                              }
                              else {
                                tft.setCursor(0,1); tft.print("17. sustain note ON"); 
                              }
                              break;                           
                            case 17: 
                              clrTopText();
                              if ( trace == 1 ) { 
                                tft.setCursor(0,1); tft.print("18. graph trace ON"); 
                              }
                              else {
                                tft.setCursor(0,1); tft.print("18. graph trace OFF"); 
                              } 
                              break;
                            case 18: 
                              writeParamString(stepsDefaultsTXTarr[stepsPointer],65); // Call function to write value 
                              tft.setCursor(0,1); tft.print("19. steps:"); 
                              break;
                            case 19: 
                              clrTopText();
                              if ( velocityMode == 0 ) { 
                                tft.setCursor(0,1); tft.print("20. velocity FIXED"); 
                              }
                              else {
                                tft.setCursor(0,1); tft.print("20. velocity AUTO"); 
                              }
                              break;                             
                            case 20: 
                              writeParam(pz_offset,100); // Call function to write value 
                              tft.setCursor(0,1); tft.print("21. vel. offst:");
                              break; 
                            case 21: 
                              writeParam(fx_select,100); // Call function to write value 
                              tft.setCursor(0,1); tft.print("22. select f(x):");
                              break;                            
                                                                                                                           
                          }            
        
                        displayCounter = 1;
                        updownPressed = 0 ;
                        toNormal = 0;
                                      
                        }                
                  
                }

                if ( operationMode == 1 ) {

                    if ( showParam.pressed() ) {
  
                        // Shuffle subset of notes -------------------------------------------------------------------  
                        for (int aNotes=0; aNotes<midiSubSelectionNotesPointer; aNotes++)
                        {
                         int rNotes = random(aNotes,midiSubSelectionNotesPointer);
                         int tempNotes = midiSubSelectionNotesArr[aNotes];
                         midiSubSelectionNotesArr[aNotes] = midiSubSelectionNotesArr[rNotes];
                         midiSubSelectionNotesArr[rNotes] = tempNotes;
                        }

                        notesShuffled = 1;                        
                      
                    }
          
                }

                if ( operationMode == 2 ) {

                    if ( showParam.pressed() ) {
  
                        // Shuffle subset of intervals ---------------------------------------------------------------  
                        for (int aInterval=0; aInterval<9; aInterval++)
                        {
                         int rInterval = random(aInterval,10);
                         int tempInterval = midiSubSelectionIntervalArr[aInterval];
                         midiSubSelectionIntervalArr[aInterval] = midiSubSelectionIntervalArr[rInterval];
                         midiSubSelectionIntervalArr[rInterval] = tempInterval;
                        }

                        storeShuffledinterval();
                      
                    }
          
                }
                // Read Menu Switches END ==================================================================================
            
            
                // Read encoder Switch START ===========================================================================
                if (encoderSwitch.pressed() ) {

                      if ( operationMode == 0 ) {
                        
                        clrScreen();
                        startPattern(); 
                        
                      }

                      if ( operationMode == 1 ) {

                            // NOTE ENA/DIS
                            if ( EnaDisNotesArr[gridSelection] == 0 ) {
      
                                  EnaDisNotesArr[gridSelection] = 1;
                                 
                              }
                              else {

                                  EnaDisNotesArr[gridSelection] = 0;
                                
                              }

                            notesShuffled = 0;
                            snapShotRetrieved = 0;  
                            drawGridSquaresNotes();                        
                        
                      }

                      if ( operationMode == 2 ) {

                             // INTERVAL ENA/DIS
                            if ( EnaDisIntervalArr[gridSelectionInterval] == 0 ) {

                                  // Check in what column the change is made then disable current interval and enable new interval
                                  
                                      // Column 1 START ===============================================
                                      for( int i = 0; i <= 80;  i=i+10 ) {
                                      
                                        if ( i == gridSelectionInterval ) { 
                                          
                                              // Disable current selection  
                                              for( int q = 108; q < 198;  q=q+10 ) {
                                                if ( EEPROM.read(q) == 1 ) {
                                                  EEPROM.write(q, 0);
                                                  EEPROM.commit();
                                                  EnaDisIntervalArr[q-108] = 0;    
                                                }
                                              }
    
                                        // Enable current selection
                                        EEPROM.write(gridSelectionInterval+108, 1);
                                        EEPROM.commit();
                                        }
                                        
                                      }                                      
                                      // Column 1 END =================================================

                                        // Column 2 START ===============================================
                                        for( int i = 1; i <= 81;  i=i+10 ) {
                                        
                                          if ( i == gridSelectionInterval ) { 
                                            
                                                // Disable current selection  
                                                for( int q = 109; q < 198;  q=q+10 ) {
                                                  if ( EEPROM.read(q) == 1 ) {
                                                    EEPROM.write(q, 0);
                                                    EEPROM.commit();
                                                    EnaDisIntervalArr[q-108] = 0;    
                                                  }
                                                }
      
                                          // Enable current selection
                                          EEPROM.write(gridSelectionInterval+108, 1);
                                          EEPROM.commit();
                                          }
                                          
                                        }
                                        // Column 2 END =================================================

                                          // Column 3 START ===============================================
                                          for( int i = 2; i <= 82;  i=i+10 ) {
                                          
                                            if ( i == gridSelectionInterval ) { 
                                              
                                                  // Disable current selection  
                                                  for( int q = 110; q < 198;  q=q+10 ) {
                                                    if ( EEPROM.read(q) == 1 ) {
                                                      EEPROM.write(q, 0);
                                                      EEPROM.commit();
                                                      EnaDisIntervalArr[q-108] = 0;    
                                                    }
                                                  }
        
                                            // Enable current selection
                                            EEPROM.write(gridSelectionInterval+108, 1);
                                            EEPROM.commit();
                                            }
                                            
                                          }
                                          // Column 3 END =================================================

                                            // Column 4 START ===============================================
                                            for( int i = 3; i <= 83;  i=i+10 ) {
                                            
                                              if ( i == gridSelectionInterval ) { 
                                                
                                                    // Disable current selection  
                                                    for( int q = 111; q < 198;  q=q+10 ) {
                                                      if ( EEPROM.read(q) == 1 ) {
                                                        EEPROM.write(q, 0);
                                                        EEPROM.commit();
                                                        EnaDisIntervalArr[q-108] = 0;    
                                                      }
                                                    }
          
                                              // Enable current selection
                                              EEPROM.write(gridSelectionInterval+108, 1);
                                              EEPROM.commit();
                                              }
                                              
                                            }
                                            // Column 4 END =================================================

                                              // Column 5 START ===============================================
                                              for( int i = 4; i <= 84;  i=i+10 ) {
                                              
                                                if ( i == gridSelectionInterval ) { 
                                                  
                                                      // Disable current selection  
                                                      for( int q = 112; q < 198;  q=q+10 ) {
                                                        if ( EEPROM.read(q) == 1 ) {
                                                          EEPROM.write(q, 0);
                                                          EEPROM.commit();
                                                          EnaDisIntervalArr[q-108] = 0;    
                                                        }
                                                      }
            
                                                // Enable current selection
                                                EEPROM.write(gridSelectionInterval+108, 1);
                                                EEPROM.commit();
                                                }
                                                
                                              }
                                              // Column 5 END =================================================

                                                // Column 6 START ===============================================
                                                for( int i = 5; i <= 85;  i=i+10 ) {
                                                
                                                  if ( i == gridSelectionInterval ) { 
                                                    
                                                        // Disable current selection  
                                                        for( int q = 113; q < 198;  q=q+10 ) {
                                                          if ( EEPROM.read(q) == 1 ) {
                                                            EEPROM.write(q, 0);
                                                            EEPROM.commit();
                                                            EnaDisIntervalArr[q-108] = 0;    
                                                          }
                                                        }
              
                                                  // Enable current selection
                                                  EEPROM.write(gridSelectionInterval+108, 1);
                                                  EEPROM.commit();
                                                  }
                                                  
                                                }
                                                // Column 6 END =================================================
                                              

                                                  // Column 7 START ===============================================
                                                  for( int i = 6; i <= 86;  i=i+10 ) {
                                                  
                                                    if ( i == gridSelectionInterval ) { 
                                                      
                                                          // Disable current selection  
                                                          for( int q = 114; q < 198;  q=q+10 ) {
                                                            if ( EEPROM.read(q) == 1 ) {
                                                              EEPROM.write(q, 0);
                                                              EEPROM.commit();
                                                              EnaDisIntervalArr[q-108] = 0;    
                                                            }
                                                          }
                
                                                    // Enable current selection
                                                    EEPROM.write(gridSelectionInterval+108, 1);
                                                    EEPROM.commit();
                                                    }
                                                    
                                                  }
                                                  // Column 7 END =================================================

                                                    // Column 8 START ===============================================
                                                    for( int i = 7; i <= 87;  i=i+10 ) {
                                                    
                                                      if ( i == gridSelectionInterval ) { 
                                                        
                                                            // Disable current selection  
                                                            for( int q = 115; q < 198;  q=q+10 ) {
                                                              if ( EEPROM.read(q) == 1 ) {
                                                                EEPROM.write(q, 0);
                                                                EEPROM.commit();
                                                                EnaDisIntervalArr[q-108] = 0;    
                                                              }
                                                            }
                  
                                                      // Enable current selection
                                                      EEPROM.write(gridSelectionInterval+108, 1);
                                                      EEPROM.commit();
                                                      }
                                                      
                                                    }
                                                    // Column 8 END =================================================
  
                                                      // Column 9 START ===============================================
                                                      for( int i = 8; i <= 88;  i=i+10 ) {
                                                      
                                                        if ( i == gridSelectionInterval ) { 
                                                          
                                                              // Disable current selection  
                                                              for( int q = 116; q < 198;  q=q+10 ) {
                                                                if ( EEPROM.read(q) == 1 ) {
                                                                  EEPROM.write(q, 0);
                                                                  EEPROM.commit();
                                                                  EnaDisIntervalArr[q-108] = 0;    
                                                                }
                                                              }
                    
                                                        // Enable current selection
                                                        EEPROM.write(gridSelectionInterval+108, 1);
                                                        EEPROM.commit();
                                                        }
                                                        
                                                      }
                                                      // Column 9 END =================================================

                                                        // Column 10 START ===============================================
                                                        for( int i = 9; i <= 89;  i=i+10 ) {
                                                        
                                                          if ( i == gridSelectionInterval ) { 
                                                            
                                                                // Disable current selection  
                                                                for( int q = 117; q < 198;  q=q+10 ) {
                                                                  if ( EEPROM.read(q) == 1 ) {
                                                                    EEPROM.write(q, 0);
                                                                    EEPROM.commit();
                                                                    EnaDisIntervalArr[q-108] = 0;    
                                                                  }
                                                                }
                      
                                                          // Enable current selection
                                                          EEPROM.write(gridSelectionInterval+108, 1);
                                                          EEPROM.commit();
                                                          }
                                                          
                                                        }
                                                        // Column 10 END =================================================                              
                                  
                              EnaDisIntervalArr[gridSelectionInterval] = 1;
                              snapShotRetrieved = 0; 
                              drawGridSquaresInterval();
                                                          
                            }                          
                        
                      }
                                        
                  }
                // Read encoder Switch END ================================================================================


    // Read encoder =======================================================================================
    Enc_currentStateCLK = digitalRead(Enc_CLK);
    
    if (Enc_currentStateCLK != Enc_previousStateCLK) {

      
    
      if ( digitalRead(Enc_DT) != Enc_currentStateCLK )
        { 
                    
          // ENCODER PARAMETER VALUE UP ========================================== START

          if ( operationMode == 0 ) {

                displayCounter = 1;
                switch (parameterSelection) {
                  case 0:
                    a_fl = a_fl + 0.5; 
                    if ( a_fl >= 500.0 ) { a_fl = 500.0; }
                    a = a_fl;                                        
                    if ( param_a == 0 ) { tft.setTextColor(PINK); } writeParam(a,100); tft.setTextColor(WHITE);  
                    tft.setCursor(0,1); if ( param_a == 0 ) { tft.setTextColor(PINK); } tft.print("1. a f(x) val:"); tft.setTextColor(WHITE); 
                    break;                             
                  case 1:                     
                    b_fl = b_fl + 0.5;
                    if ( b_fl >= 500.0 ) { b_fl = 500.0; }
                    b = b_fl;                    
                    if ( param_b == 0 ) { tft.setTextColor(PINK); } writeParam(b,100); tft.setTextColor(WHITE); 
                    tft.setCursor(0,1); if ( param_b == 0 ) { tft.setTextColor(PINK); } tft.print("2. b f(x) val:"); tft.setTextColor(WHITE);  
                    break;
                  case 2:
                    c_fl = c_fl + 0.5;
                    if ( c_fl >= 500.0 ) { c_fl = 500.0; }
                    c = c_fl;                    
                    if ( param_c == 0 ) { tft.setTextColor(PINK); } writeParam(c,100); tft.setTextColor(WHITE); 
                    tft.setCursor(0,1); if ( param_c == 0 ) { tft.setTextColor(PINK); } tft.print("3. c f(x) val:"); tft.setTextColor(WHITE); 
                    break;
                  case 3:
                    d_fl = d_fl + 0.5;
                    if ( d_fl >= 500.0 ) { d_fl = 500.0; }
                    d = d_fl;                    
                    if ( param_d == 0 ) { tft.setTextColor(PINK); } writeParam(d,100); tft.setTextColor(WHITE);  
                    tft.setCursor(0,1); if ( param_d == 0 ) { tft.setTextColor(PINK); } tft.print("4. d f(x) val:"); tft.setTextColor(WHITE);  
                    break;
                  case 4:
                    rnd_fl = rnd_fl + 0.5;
                    if ( rnd_fl >= 500.0 ) { rnd_fl = 500.0; }
                    rnd = rnd_fl;                    
                    if ( param_rnd == 0 ) { tft.setTextColor(PINK); } writeParam(rnd,100); tft.setTextColor(WHITE);  
                    tft.setCursor(0,1); if ( param_rnd == 0 ) { tft.setTextColor(PINK); } tft.print("5. rnd f(x) val:"); tft.setTextColor(WHITE);  
                    break;                                 
                  case 5:                              
                    delta_gran_fl = delta_gran_fl + 0.5;
                    if ( delta_gran_fl >= 999 ) { delta_gran_fl = 999.0; } 
                    delta_gran = delta_gran_fl;                    
                    value_gran = delta_gran_fl / 2000;              
                    writeParam(delta_gran,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("6. t step size:"); 
                    break;
                  case 6: 
                    x_fl = x_fl + 0.5;
                    x = x_fl; 
                    if ( x >= 128 ) { x = 128; x_fl = 128.0; }             
                    writeParam(x,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("7. x int offst:");
                    break;
                  case 7:                                          
                    y_fl = y_fl + 0.5;
                    y = y_fl;
                    if ( y >= 128 ) { y = 128; y_fl = 128.0; }
                    writeParam(y,105); // Call function to write value 
                    tft.setCursor(0,1); tft.print("8. y ptch offst:"); 
                    break;                                  
                  case 8: 
                    value_t1_fl = value_t1_fl + 0.5;
                    if ( value_t1_fl >= 500 ) { value_t1_fl = 500.0; } 
                    value_t1 = value_t1_fl;
                    writeParam(value_t1,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("9. t from (min):");
                    break; 
                  case 9: 
                    value_t2_fl = value_t2_fl + 0.5;
                    if ( value_t2_fl >= 500 ) { value_t2_fl = 500.0; }
                    value_t2 = value_t2_fl;
                    writeParam(value_t2,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("10. t to (max):");
                    break;  
                  case 10: 
                    pyRest_fl = pyRest_fl + 0.5;
                    if ( pyRest_fl  >= 127.0 ) { pyRest_fl = 127.0; }
                    pyRest = pyRest_fl;              
                    writeParam(pyRest,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("11. set mask:");
                    if ( triggerMode >= 1 ) { drawTriUp(pyRest); } else { drawTriDown(pyRest); } 
                    break;
                  case 11: 
                    triggerMode_fl = triggerMode_fl + 0.5;
                    triggerMode = triggerMode_fl;
                      if ( triggerMode >= 1 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("12. notes ABOVE mask"); triggerMode = 1; triggerMode_fl = 1.0; 
                      }
                    break;
                  case 12: 
                    thruMode_fl = thruMode_fl + 0.5;
                    thruMode = thruMode_fl;
                      if ( thruMode >= 1 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("13. MIDI Thru OFF"); thruMode = 1; thruMode_fl = 1.0;
                        MIDI.turnThruOff();  
                      }
                    break;
                  case 13: 
                    midi_channel_fl = midi_channel_fl + 0.5;
                    midi_channel = midi_channel_fl; 
                    if ( midi_channel >= 16 ) { midi_channel = 16; midi_channel_fl = 16.0; }             
                    writeParam(midi_channel,105); // Call function to write value 
                    tft.setCursor(0,1); tft.print("14. MIDI Tx Chnl:");
                    break;
                  case 14: 
                    startStopFromMenu_fl = startStopFromMenu_fl + 0.5;
                    startStopFromMenu = startStopFromMenu_fl;
                      if ( startStopFromMenu == 0 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("15. Start/Stop AUTO"); 
                      }
                      if ( startStopFromMenu >= 1 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("15. Start/Stop START"); startStopFromMenu = 1; startStopFromMenu_fl = 1.0; DimensionsGo = 1; startStopFromMenu = 0;
                      }
                    break; 
                  case 15:                
                    octTranspose_fl = octTranspose_fl + 0.5;
                    octTranspose = octTranspose_fl;
                    if ( octTranspose >= 24 ) { octTranspose = 24; octTranspose_fl = 24.0; }                    
                    writeParam(octTranspose,100); // Call function to write value  
                    tft.setCursor(0,1); tft.print("16. transpose:"); 
                    break;
                  case 16: 
                    sustainNoteMode_fl = sustainNoteMode_fl + 0.5;
                    sustainNoteMode = sustainNoteMode_fl;
                      if ( sustainNoteMode >= 1 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("17. sustain note ON"); sustainNoteMode = 1; sustainNoteMode_fl = 1.0; 
                      }
                    break; 
                  case 17: 
                    trace_fl = trace_fl + 0.5;
                    trace = trace_fl;
                      if ( trace >= 1 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("18. graph trace ON"); trace = 1; trace_fl = 1.0; 
                      }
                    break;
                  case 18:                
                    stepsPointer_fl = stepsPointer_fl + 0.5;
                    stepsPointer = stepsPointer_fl;
                    if ( stepsPointer >= 7 ) { stepsPointer = 7; stepsPointer_fl = 7.0; }                    
                    writeParamString(stepsDefaultsTXTarr[stepsPointer],65); // Call function to write value  
                    tft.setCursor(0,1); tft.print("19. steps:"); 
                    break;
                  case 19: 
                    velocityMode_fl = velocityMode_fl + 0.5;
                    velocityMode = velocityMode_fl;
                      if ( velocityMode >= 1 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("20. velocity AUTO"); velocityMode = 1; velocityMode_fl = 1.0; 
                      }
                    break;                    
                  case 20: 
                    pz_offset_fl = pz_offset_fl + 0.5;
                    if ( pz_offset_fl >= 127.0 ) { pz_offset_fl = 127.0; }
                    pz_offset = pz_offset_fl;              
                    writeParam(pz_offset,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("21. vel. offst:");
                    break;
                  case 21:                
                    fx_select_fl = fx_select_fl + 0.5;
                    fx_select = fx_select_fl;
                    if ( fx_select >= numberOfParametricEQs ) { fx_select = numberOfParametricEQs; fx_select_fl = numberOfParametricEQs_fl; }                    
                    writeParam(fx_select,100); // Call function to write value  
                    tft.setCursor(0,1); tft.print("22. select f(x):"); 
                    break; 
                                                                    
                  }

              }

              if ( operationMode == 1 ) {

                // NOTE ENA/DIS
                gridSelection_fl = gridSelection_fl + 0.5;
                gridSelection = gridSelection_fl;
                if ( gridSelection > 107 ) { gridSelection = 0; gridSelection_fl = 0.0; }
                
                  gridSelectionBox_X = gridSelection * 10;   
                  
                  if ( gridSelection == 0 ) { tft.drawRect(119, 109, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X+9, 29, 9, 9, WHITE); } 
                                                     
                  if ( gridSelection > 0 && gridSelection < 12 ) { tft.drawRect(gridSelectionBox_X-1, 29, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X+9, 29, 9, 9, WHITE); }

                  if ( gridSelection == 12 ) { tft.drawRect(119, 29, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-111, 39, 9, 9, WHITE); } 
                                
                  if ( gridSelection > 12 && gridSelection < 24 ) { tft.drawRect(gridSelectionBox_X-121, 39, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-111, 39, 9, 9, WHITE); }
                  
                  if ( gridSelection == 24 ) { tft.drawRect(119, 39, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-231, 49, 9, 9, WHITE); }
                   
                  if ( gridSelection > 24 && gridSelection < 36 ) { tft.drawRect(gridSelectionBox_X-241, 49, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-231, 49, 9, 9, WHITE); }
                  
                  if ( gridSelection == 36 ) { tft.drawRect(119, 49, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-351, 59, 9, 9, WHITE); }
                   
                  if ( gridSelection > 36 && gridSelection < 48 ) { tft.drawRect(gridSelectionBox_X-361, 59, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-351, 59, 9, 9, WHITE); }
                  
                  if ( gridSelection == 48 ) { tft.drawRect(119, 59, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-471, 69, 9, 9, WHITE); }
                  
                  if ( gridSelection > 48 && gridSelection < 60 ) { tft.drawRect(gridSelectionBox_X-481, 69, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-471, 69, 9, 9, WHITE); }

                  if ( gridSelection == 60 ) { tft.drawRect(119, 69, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-591, 79, 9, 9, WHITE); }
                  
                  if ( gridSelection > 60 && gridSelection < 72 ) { tft.drawRect(gridSelectionBox_X-601, 79, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-591, 79, 9, 9, WHITE); }

                  if ( gridSelection == 72 ) { tft.drawRect(119, 79, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-711, 89, 9, 9, WHITE); }
                  
                  if ( gridSelection > 72 && gridSelection < 84 ) { tft.drawRect(gridSelectionBox_X-721, 89, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-711, 89, 9, 9, WHITE); }
                  
                  if ( gridSelection == 84 ) { tft.drawRect(119, 89, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-831, 99, 9, 9, WHITE); }
                  
                  if ( gridSelection > 84 && gridSelection < 96 ) { tft.drawRect(gridSelectionBox_X-841, 99, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-831, 99, 9, 9, WHITE); }
                  
                  if ( gridSelection == 96 ) { tft.drawRect(119, 99, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-951, 109, 9, 9, WHITE); }
                  
                  if ( gridSelection > 96 && gridSelection < 108 ) { tft.drawRect(gridSelectionBox_X-961, 109, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-951, 109, 9, 9, WHITE); }  

                  //if ( gridSelection == 107 ) { tft.drawRect(109, 109, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-951, 109, 9, 9, WHITE); }                                                         
                
              }

              if ( operationMode == 2 ) {
    
                // INTERVAL ENA/DIS
                gridSelectionInterval_fl = gridSelectionInterval_fl + 0.5;
                gridSelectionInterval = gridSelectionInterval_fl;
                if ( gridSelectionInterval > 89 ) { gridSelectionInterval = 0; gridSelectionInterval_fl = 0.0; }
                
                  gridSelectionBoxInterval_X = gridSelectionInterval * 10;   
                  
                  if ( gridSelectionInterval == 0 ) { tft.drawRect(119, 109, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X+29, 29, 9, 9, WHITE); } 
                                                     
                  if ( gridSelectionInterval > 0 && gridSelectionInterval < 10 ) { tft.drawRect(gridSelectionBoxInterval_X+19, 29, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X+29, 29, 9, 9, WHITE); }
    
                  if ( gridSelectionInterval == 10 ) { tft.drawRect(119, 29, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-71, 39, 9, 9, WHITE); } 
                               
                  if ( gridSelectionInterval > 10 && gridSelectionInterval < 20 ) { tft.drawRect(gridSelectionBoxInterval_X-81, 39, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-71, 39, 9, 9, WHITE); }
                  
                  if ( gridSelectionInterval == 20 ) { tft.drawRect(119, 39, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-171, 49, 9, 9, WHITE); }
                   
                  if ( gridSelectionInterval > 20 && gridSelectionInterval < 30 ) { tft.drawRect(gridSelectionBoxInterval_X-181, 49, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-171, 49, 9, 9, WHITE); }
                  
                  if ( gridSelectionInterval == 30 ) { tft.drawRect(119, 49, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-271, 59, 9, 9, WHITE); }
                   
                  if ( gridSelectionInterval > 30 && gridSelectionInterval < 40 ) { tft.drawRect(gridSelectionBoxInterval_X-281, 59, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-271, 59, 9, 9, WHITE); }                  
                  
                  if ( gridSelectionInterval == 40 ) { tft.drawRect(119, 59, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-371, 69, 9, 9, WHITE); }
                  
                  if ( gridSelectionInterval > 40 && gridSelectionInterval < 50 ) { tft.drawRect(gridSelectionBoxInterval_X-381, 69, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-371, 69, 9, 9, WHITE); }             
    
                  if ( gridSelectionInterval == 50 ) { tft.drawRect(119, 69, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-471, 79, 9, 9, WHITE); }
                  
                  if ( gridSelectionInterval > 50 && gridSelectionInterval < 60 ) { tft.drawRect(gridSelectionBoxInterval_X-481, 79, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-471, 79, 9, 9, WHITE); }                 
    
                  if ( gridSelectionInterval == 60 ) { tft.drawRect(119, 79, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-571, 89, 9, 9, WHITE); }
                  
                  if ( gridSelectionInterval > 60 && gridSelectionInterval < 70 ) { tft.drawRect(gridSelectionBoxInterval_X-581, 89, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-571, 89, 9, 9, WHITE); }             
                  
                  if ( gridSelectionInterval == 70 ) { tft.drawRect(119, 89, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-671, 99, 9, 9, WHITE); }
                  
                  if ( gridSelectionInterval > 70 && gridSelectionInterval < 80 ) { tft.drawRect(gridSelectionBoxInterval_X-681, 99, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-671, 99, 9, 9, WHITE); }             
                  
                  if ( gridSelectionInterval == 80 ) { tft.drawRect(119, 99, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-771, 109, 9, 9, WHITE); }                
                  
                  if ( gridSelectionInterval > 80 && gridSelectionInterval < 90 ) { tft.drawRect(gridSelectionBoxInterval_X-781, 109, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-771, 109, 9, 9, WHITE); }
                
              }




                          
          }
          // ENCODER PARAMETER VALUE UP ========================================== END
      else 
        { 
          // ENCODER PARAMETER VALUE DOWN ========================================== START

          if ( operationMode == 0 ) {

                displayCounter = 1;
                switch (parameterSelection) {
                  case 0:
                    a_fl = a_fl - 0.5;
                    if ( a_fl <= -500.0 ) { a_fl = -500.0; } 
                    a = a_fl;
                    tft.setCursor(0,1); if ( param_a == 0 ) { tft.setTextColor(PINK); } writeParam(a,100); tft.setTextColor(WHITE); 
                    tft.setCursor(0,1); if ( param_a == 0 ) { tft.setTextColor(PINK); } tft.setCursor(0,1); tft.print("1. a f(x) val:"); tft.setTextColor(WHITE); 
                    break;                             
                  case 1:                     
                    b_fl = b_fl - 0.5;
                    if ( b_fl <= -500.0 ) { b_fl = -500.0; } 
                    b = b_fl;
                    tft.setCursor(0,1); if ( param_b == 0 ) { tft.setTextColor(PINK); } writeParam(b,100); tft.setTextColor(WHITE); 
                    tft.setCursor(0,1); if ( param_b == 0 ) { tft.setTextColor(PINK); } tft.setCursor(0,1); tft.print("2. b f(x) val:"); tft.setTextColor(WHITE);
                    break;
                  case 2:
                    c_fl = c_fl - 0.5;
                    if ( c_fl <= -500.0 ) { c_fl = -500.0; } 
                    c = c_fl;
                    tft.setCursor(0,1); if ( param_c == 0 ) { tft.setTextColor(PINK); } writeParam(c,100); tft.setTextColor(WHITE); 
                    tft.setCursor(0,1); if ( param_c == 0 ) { tft.setTextColor(PINK); } tft.setCursor(0,1); tft.print("3. c f(x) val:"); tft.setTextColor(WHITE);
                    break;
                  case 3:
                    d_fl = d_fl - 0.5;
                    if ( d_fl <= -500.0 ) { d_fl = -500.0; } 
                    d = d_fl;
                    tft.setCursor(0,1); if ( param_d == 0 ) { tft.setTextColor(PINK); } writeParam(d,100); tft.setTextColor(WHITE); 
                    tft.setCursor(0,1); if ( param_d == 0 ) { tft.setTextColor(PINK); } tft.setCursor(0,1); tft.print("4. d f(x) val:"); tft.setTextColor(WHITE); 
                    break;
                  case 4:
                    rnd_fl = rnd_fl - 0.5;
                    if ( rnd_fl <= -500.0 ) { rnd_fl = -500.0; } 
                    rnd = rnd_fl;
                    tft.setCursor(0,1); if ( param_rnd == 0 ) { tft.setTextColor(PINK); } writeParam(rnd,100); tft.setTextColor(WHITE); 
                    tft.setCursor(0,1); if ( param_rnd == 0 ) { tft.setTextColor(PINK); } tft.setCursor(0,1); tft.print("5. rnd f(x) val:"); tft.setTextColor(WHITE); 
                    break;                    
                  case 5:                     
                    delta_gran_fl = delta_gran_fl - 0.5;
                    if ( delta_gran_fl <= -999 ) { delta_gran_fl = -999.0; } 
                    delta_gran = delta_gran_fl;
                    value_gran = delta_gran_fl / 2000;              
                    writeParam(delta_gran,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("6. t step size:"); 
                    break;
                  case 6: 
                    x_fl = x_fl - 0.5;
                    x = x_fl;
                    if ( x <= 1 ) { x = 1; x_fl = 1.0; }
                    writeParam(x,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("7. x int offst:");              
                    break;
                  case 7:                
                    y_fl = y_fl - 0.5;
                    y = y_fl;
                    if ( y <= 1 ) { y = 1; y_fl = 1.0; }
                    writeParam(y,105); // Call function to write value 
                    tft.setCursor(0,1); tft.print("8. y ptch offst:"); 
                    break;                                
                  case 8: 
                    value_t1_fl = value_t1_fl - 0.5;
                    if ( value_t1_fl <= -500 ) { value_t1_fl = -500.0; } 
                    value_t1 = value_t1_fl;
                    writeParam(value_t1,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("9. t from (min):");
                    break; 
                  case 9: 
                    value_t2_fl = value_t2_fl - 0.5;
                    if ( value_t2_fl <= -500 ) { value_t2_fl = -500.0; } 
                    value_t2 = value_t2_fl;
                    writeParam(value_t2,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("10. t to (max):");
                    break;  
                  case 10: 
                    pyRest_fl = pyRest_fl - 0.5;
                    if ( pyRest_fl  <= 0.0 ) { pyRest_fl = 0.0; }
                    pyRest = pyRest_fl;              
                    writeParam(pyRest,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("11. set mask:");
                    if ( triggerMode <= 0 ) { drawTriDown(pyRest); } else { drawTriUp(pyRest); } 
                    break;
                  case 11: 
                    triggerMode_fl = triggerMode_fl - 0.5;
                    triggerMode = triggerMode_fl;
                      if ( triggerMode <= 0 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("12. notes BELOW mask"); triggerMode = 0; triggerMode_fl = 0.0; 
                      }
                    break;
                  case 12: 
                    thruMode_fl = thruMode_fl - 0.5;
                    thruMode = thruMode_fl;
                      if ( thruMode <= 0 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("13. MIDI Thru ON"); thruMode = 0; thruMode_fl = 0.0;
                        MIDI.turnThruOn();  
                      }
                    break;
                  case 13: 
                    midi_channel_fl = midi_channel_fl - 0.5;
                    midi_channel = midi_channel_fl; 
                    if ( midi_channel <= 1 ) { midi_channel = 1; midi_channel_fl = 1.0; }             
                    writeParam(midi_channel,105); // Call function to write value 
                    tft.setCursor(0,1); tft.print("14. MIDI Tx Chnl:");
                    break;
                  case 14: 
                    startStopFromMenu_fl = startStopFromMenu_fl - 0.5;
                    startStopFromMenu = startStopFromMenu_fl;
                      if ( startStopFromMenu == 0 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("15. Start/Stop AUTO"); 
                      }                    
                      if ( startStopFromMenu <= -1 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("15. Start/Stop STOP"); startStopFromMenu = -1; startStopFromMenu_fl = -1.0; DimensionsGo = 0; startStopFromMenu = 0;
                      }
                    break;    
                  case 15:                
                    octTranspose_fl = octTranspose_fl - 0.5;
                    octTranspose = octTranspose_fl;
                    if ( octTranspose <= -24 ) { octTranspose = -24; octTranspose_fl = -24.0; }                    
                    writeParam(octTranspose,100); // Call function to write value  
                    tft.setCursor(0,1); tft.print("16. transpose:"); 
                    break;
                  case 16: 
                    sustainNoteMode_fl = sustainNoteMode_fl - 0.5;
                    sustainNoteMode = sustainNoteMode_fl;
                      if ( sustainNoteMode <= 0 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("17. sustain note OFF"); sustainNoteMode = 0; sustainNoteMode_fl = 0.0; 
                      }
                    break;
                  case 17: 
                    trace_fl = trace_fl - 0.5;
                    trace = trace_fl;
                      if ( trace <= 0 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("18. graph trace OFF"); trace = 0; trace_fl = 0.0;
                      }
                    break;
                  case 18:                
                    stepsPointer_fl = stepsPointer_fl - 0.5;
                    stepsPointer = stepsPointer_fl;
                    if ( stepsPointer <= 0 ) { stepsPointer = 0; stepsPointer_fl = 0.0; }               
                    writeParamString(stepsDefaultsTXTarr[stepsPointer],65); // Call function to write value  
                    tft.setCursor(0,1); tft.print("19. steps:"); 
                    break;
                  case 19: 
                    velocityMode_fl = velocityMode_fl - 0.5;
                    velocityMode = velocityMode_fl;
                      if ( velocityMode <= 0 ) {
                        clrTopText();
                        tft.setCursor(0,1); tft.print("20. velocity FIXED"); velocityMode = 0; velocityMode_fl = 0.0; 
                      }
                    break;                    
                  case 20: 
                    pz_offset_fl = pz_offset_fl - 0.5;
                    if ( pz_offset_fl <= 0.0 ) { pz_offset_fl = 0.0; }
                    pz_offset = pz_offset_fl;
                    writeParam(pz_offset,100); // Call function to write value 
                    tft.setCursor(0,1); tft.print("21. vel. offst:");
                    break;
                  case 21:                
                    fx_select_fl = fx_select_fl - 0.5;
                    fx_select = fx_select_fl;
                    if ( fx_select <= 1 ) { fx_select = 1; fx_select_fl = 1.0; }                    
                    writeParam(fx_select,100); // Call function to write value  
                    tft.setCursor(0,1); tft.print("22. select f(x):"); 
                    break;
                                  
                  }

              }

              if ( operationMode == 1 ) {

                // NOTE ENA/DIS
                gridSelection_fl = gridSelection_fl - 0.5;
                gridSelection = gridSelection_fl;
                if ( gridSelection < 0 ) { gridSelection = 107; gridSelection_fl = 107.0; }

                  gridSelectionBox_X = gridSelection * 10;   
                  
                  if ( gridSelection == 107 ) { tft.drawRect(9, 29, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-951, 109, 9, 9, WHITE); } 
                                                     
                  if ( gridSelection < 107 && gridSelection > 95 ) { tft.drawRect(gridSelectionBox_X-941, 109, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-951, 109, 9, 9, WHITE); }

                  if ( gridSelection == 95 ) { tft.drawRect(9, 109, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-831, 99, 9, 9, WHITE); }
   
                  if ( gridSelection < 95 && gridSelection > 83 ) { tft.drawRect(gridSelectionBox_X-821, 99, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-831, 99, 9, 9, WHITE); }

                  if ( gridSelection == 83 ) { tft.drawRect(9, 99, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-711, 89, 9, 9, WHITE); }

                  if ( gridSelection < 83 && gridSelection > 71 ) { tft.drawRect(gridSelectionBox_X-701, 89, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-711, 89, 9, 9, WHITE); }
                  
                  if ( gridSelection == 71 ) { tft.drawRect(9, 89, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-591, 79, 9, 9, WHITE); }

                  if ( gridSelection < 71 && gridSelection > 59 ) { tft.drawRect(gridSelectionBox_X-581, 79, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-591, 79, 9, 9, WHITE); }

                  if ( gridSelection == 59 ) { tft.drawRect(9, 79, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-471, 69, 9, 9, WHITE); }

                  if ( gridSelection < 59 && gridSelection > 47 ) { tft.drawRect(gridSelectionBox_X-461, 69, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-471, 69, 9, 9, WHITE); }

                  if ( gridSelection == 47 ) { tft.drawRect(9, 69, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-351, 59, 9, 9, WHITE); }

                  if ( gridSelection < 47 && gridSelection > 35 ) { tft.drawRect(gridSelectionBox_X-341, 59, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-351, 59, 9, 9, WHITE); }

                  if ( gridSelection == 35 ) { tft.drawRect(9, 59, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-231, 49, 9, 9, WHITE); }

                  if ( gridSelection < 35 && gridSelection > 23 ) { tft.drawRect(gridSelectionBox_X-221, 49, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-231, 49, 9, 9, WHITE); }

                  if ( gridSelection == 23 ) { tft.drawRect(9, 49, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-111, 39, 9, 9, WHITE); }

                  if ( gridSelection < 23 && gridSelection > 11 ) { tft.drawRect(gridSelectionBox_X-101, 39, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X-111, 39, 9, 9, WHITE); }

                  if ( gridSelection == 11 ) { tft.drawRect(9, 39, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X+9, 29, 9, 9, WHITE); }

                  if ( gridSelection < 11 && gridSelection > 0 ) { tft.drawRect(gridSelectionBox_X+19, 29, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X+9, 29, 9, 9, WHITE); }
                  
                  if ( gridSelection == 0 ) { tft.drawRect(19, 29, 9, 9, BLACK); tft.drawRect(gridSelectionBox_X+9, 29, 9, 9, WHITE); }
                
              }

              if ( operationMode == 2 ) {
    
                // INTERVAL ENA/DIS
                gridSelectionInterval_fl = gridSelectionInterval_fl - 0.5;
                gridSelectionInterval = gridSelectionInterval_fl;
                if ( gridSelectionInterval < 0 ) { gridSelectionInterval = 89; gridSelectionInterval_fl = 89.0; }
                
                  gridSelectionBoxInterval_X = gridSelectionInterval * 10;

                  if ( gridSelectionInterval == 89 ) { tft.drawRect(29, 29, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-771, 109, 9, 9, WHITE); } 

                  if ( gridSelectionInterval < 90 && gridSelectionInterval > 79 ) { tft.drawRect(gridSelectionBoxInterval_X-761, 109, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-771, 109, 9, 9, WHITE); }

                  if ( gridSelectionInterval == 79 ) { tft.drawRect(29, 109, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-671, 99, 9, 9, WHITE); }
   
                  if ( gridSelectionInterval < 79 && gridSelectionInterval > 69 ) { tft.drawRect(gridSelectionBoxInterval_X-661, 99, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-671, 99, 9, 9, WHITE); }

                  if ( gridSelectionInterval == 69 ) { tft.drawRect(29, 99, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-571, 89, 9, 9, WHITE); }

                  if ( gridSelectionInterval < 69 && gridSelectionInterval > 59 ) { tft.drawRect(gridSelectionBoxInterval_X-561, 89, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-571, 89, 9, 9, WHITE); }
                  
                  if ( gridSelectionInterval == 59 ) { tft.drawRect(29, 89, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-471, 79, 9, 9, WHITE); }

                  if ( gridSelectionInterval < 59 && gridSelectionInterval > 49 ) { tft.drawRect(gridSelectionBoxInterval_X-461, 79, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-471, 79, 9, 9, WHITE); }

                  if ( gridSelectionInterval == 49 ) { tft.drawRect(29, 79, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-371, 69, 9, 9, WHITE); }

                  if ( gridSelectionInterval < 49 && gridSelectionInterval > 39 ) { tft.drawRect(gridSelectionBoxInterval_X-361, 69, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-371, 69, 9, 9, WHITE); }    

                  if ( gridSelectionInterval == 39 ) { tft.drawRect(29, 69, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-271, 59, 9, 9, WHITE); }

                  if ( gridSelectionInterval < 39 && gridSelectionInterval > 29 ) { tft.drawRect(gridSelectionBoxInterval_X-261, 59, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-271, 59, 9, 9, WHITE); }

                  if ( gridSelectionInterval == 29 ) { tft.drawRect(29, 59, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-171, 49, 9, 9, WHITE); }

                  if ( gridSelectionInterval < 29 && gridSelectionInterval > 19 ) { tft.drawRect(gridSelectionBoxInterval_X-161, 49, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-171, 49, 9, 9, WHITE); }

                  if ( gridSelectionInterval == 19 ) { tft.drawRect(29, 49, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-71, 39, 9, 9, WHITE); }

                  if ( gridSelectionInterval < 19 && gridSelectionInterval > 9 ) { tft.drawRect(gridSelectionBoxInterval_X-61, 39, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X-71, 39, 9, 9, WHITE); }
                  
                  if ( gridSelectionInterval == 9 ) { tft.drawRect(29, 39, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X+29, 29, 9, 9, WHITE); }

                  if ( gridSelectionInterval < 9 && gridSelectionInterval > 0 ) { tft.drawRect(gridSelectionBoxInterval_X+39, 29, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X+29, 29, 9, 9, WHITE); }
                  
                  if ( gridSelectionInterval == 0 ) { tft.drawRect(39, 29, 9, 9, BLACK); tft.drawRect(gridSelectionBoxInterval_X+29, 29, 9, 9, WHITE); }

              }
                  
          
          }
          // ENCODER PARAMETER VALUE DOWN ========================================== END

      Enc_previousStateCLK = Enc_currentStateCLK; 
      
    }






 if (MIDI.read()) {
              
              
                     switch(MIDI.getType()) // MIDI Message Type
                      {
                      
                        case midi::Start:
                          // Start Byte Detected
                          DimensionsGo = 1;
                          startStopFromMenu = 0;
                          startPattern();                          
                          break;
              
                        case midi::Stop:
                        DimensionsGo = 0;
                        startStopFromMenu = 0;
                          // Stop Byte Detected
                          break;
                        
                        case midi::Clock:
                          // Clock Byte Detected 
                          midiTimeOut = 0;

                          if ( midiTimeOutBlankScreen == 1 ) {

                              tft.setTextColor(BLACK);  
                              tft.setCursor(25, 40);     
                              tft.println("Waiting for");
                              tft.setCursor(25, 50);     
                              tft.println("MIDI clock...");
                              tft.setTextColor(WHITE); 

                            midiTimeOutBlankScreen = 0; 
                            
                          }
                          
 
           
// Run Parametric Function ======
parametricFunctions();
// ==============================
                               
                                   
                                    px = px_fl;
                                    py = py_fl;
                                    pz = pz_fl;                                   
                                   
                                   // Guard XY Boundaries ============================ 
                                   if (px <= 0) {px = 0;}
                                   if (px >= 127) {px = 127;} 
                                                                      
                                   if (py <= 0) {py = 0;}
                                   if (py >= 127) {py = 127;}                                                                     
                                   // ================================================ 

                                   // Guard Z Boundaries ============================ 
                                                                     
                                   if (pz <= 0) {pz = 1;}                                    
                                   pz = pz_offset + pz; 
                                   if (pz >= 127) {pz = 127;}
                                   

                                   // ReMap pz to a drawable circle
                                   pz_draw = map(pz,1,127,1,5);
                                   if ( velocityMode == 0 ) { pz_draw = 1; } 
                                   // ================================================ 

                                   
                                              if ( (midiClockCounter % clkDiv) == 0 ) {


                                   
                                                if ( triggerMode <= 0 && py >= pyRest ) { playThatNote(); }
                                                if ( triggerMode >= 1 && py <= pyRest ) { playThatNote(); }                                                  
                                                                                            
                                              }                                                                 


                                              // Play Animation
                                              
                                              if ( (midiClockCounter % playCircleArr[stepsPointer]) == 0 ) {
                                                drawPlayAnimation(playCircle);
                                                playCircle++;
                                                
                                                }

                              
                                 
                                  // Tail building, drawing and tracking START =================================================================================================================
                                  if ( tt-1 >= 0 ) {
  
                                   tailpx_list[tt-1] = px;
                                   tailpy_list[tt-1] = py;
                                   tailpz_list[tt-1] = pz_draw; 
                                   tt--; 
                                                                  
                                  } else {

                                      if ( operationMode == 0 ) {
                                     
                                         if ( trace <= 0 ) {
                                          tft.fillCircle(tailpx_list[taillength-1], tailpy_list[taillength-1], tailpz_list[taillength-1], BLACK); 
                                          }
                                         else {

                                                if ( NoteEnaDis == 0 || midiSubSelectionNotesPointer == 0 ) {

                                                  tft.fillCircle(tailpx_list[taillength-1], tailpy_list[taillength-1], tailpz_list[taillength-1], RED);
                                                  
                                                }
                                                else {
                                                  
                                                  if ( midiStartStop == 1 && DimensionsGo == 0 ) { 
                                                  
                                                    tft.fillCircle(tailpx_list[taillength-1], tailpy_list[taillength-1], tailpz_list[taillength-1], PURPLE);
                                                   
                                                   } else {
                                                    
                                                    tft.fillCircle(tailpx_list[taillength-1], tailpy_list[taillength-1], tailpz_list[taillength-1], YELLOW);
                                                    
                                                  }
                                                  
                                                }
                                          
                                          
                                         }

                                      }
                                 
                                          for (int i=taillength-1; i>=1; i--) {
                                            tailpx_list[i] = tailpx_list[i-1];
                                            tailpy_list[i] = tailpy_list[i-1];
                                            tailpz_list[i] = tailpz_list[i-1];                                       
                                          }    

                                     tailpx_list[0] = px;
                                     tailpy_list[0] = py;
                                     tailpz_list[0] = pz_draw;    
                                      
                                  }

                                      if ( operationMode == 0 ) {
                                    
                                         if ( Note_playNote == 1 ) {

                                          if ( NoteEnaDis == 0 || midiSubSelectionNotesPointer == 0 ) {
                                            tft.fillCircle(px, py, pz_draw, RED); 
                                          }
                                          else {

                                                  if ( midiStartStop == 1 && DimensionsGo == 0 ) {
                                                    tft.fillCircle(px, py, pz_draw, PURPLE); 
                                                     
                                                  }
                                                  else
                                                  {
                                                    tft.fillCircle(px, py, pz_draw, BLUE);   
                                                  }
                                          
                                          }
                                          
                                           
                                         }
                                         else {
                                          tft.fillCircle(px, py, pz_draw, YELLOW); 
                                         }

                                      }

                                  // Tail building, drawing and tracking END ===================================================================================================================
  
                              
                      // Control Range START ===============================================================================
                      if ( value_t1 <= value_t2 ) {

                        t = t + value_gran;
                        
                        if ( t >= value_t2 ) { t = value_t1; }

                      }
                    
                      if ( value_t1 > value_t2 ) {

                        t = t - value_gran;

                        if ( t <= value_t2 ) { t = value_t1; }
                        
                      }
                      // Control Range END =================================================================================


                          midiClockCounter++;                        
                          

                          if ( midiClockCounter > stepsDefaults[stepsPointer] ) { 
                            
                            startPattern();
                            playCircle = 0;
                                                                                                         
                            }                            
                          
                          break;
                            
                      }

                 }
 
}

// =======================================================================================================================================================
// =======================================================================================================================================================
// =======================================================================================================================================================
// =======================================================================================================================================================
// Functions =============================================================================================================================================
// =======================================================================================================================================================
// =======================================================================================================================================================
// =======================================================================================================================================================

void writeParamQ1(int valueToDisplay, int textXpos, int textYpos)
{ 
  tft.fillRect(0, 20, 128, 12, BLACK);
  tft.setCursor(textXpos,textYpos); tft.print(valueToDisplay);
}
void writeParamQ2(int valueToDisplay, int textXpos, int textYpos)
{ 
  tft.fillRect(0, 40, 128, 12, BLACK);
  tft.setCursor(textXpos,textYpos); tft.print(valueToDisplay);
}
void writeParamQ3(int valueToDisplay, int textXpos, int textYpos)
{ 
  tft.fillRect(0, 60, 128, 12, BLACK);   
  tft.setCursor(textXpos,textYpos); tft.print(valueToDisplay);
}

void writeParam(int valueToDisplay, int textXpos)
{ 
  clrTopText();
  tft.setCursor(textXpos,1); tft.print(valueToDisplay);
}

void writeParamString(String valueToDisplay, int textXpos)
{ 
  clrTopText();
  tft.setCursor(textXpos,1); tft.print(valueToDisplay);
}

void clrTopText()
{
  tft.fillRect(0, 1, 123, 10, BLACK);
}

void writeNote(String noteToDisplay, int textXpos)
{ 
  clrBottomText();
  tft.setCursor(textXpos,121); tft.print(noteToDisplay);
}

void clrBottomText()
{
  tft.fillRect(110, 121, 18, 7, BLACK);
}

void clrScreen()
{
  tft.fillScreen(BLACK);
}

void startPattern()
{  
  midiClockCounter = 0;
  playCircle = 0;
  t = value_t1;
  
        
      // Proper wipe screen before redrawing (in trace ON mode only) 
      if ( trace >= 1 ) {
        
        if ( operationMode == 0 ) { clrScreen(); }          
              
       tt = taillength; 
      } 
}

void playThatNote()
{
      
      Note_playNote = 1;     // 1 = Yes, play a note
      
      Note_playNoteTrig = 1; // 1 = Yes, Trigger note                                                


      // Determine INTERVAL and display ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
      clkDiv = midiSubSelectionIntervalArr[ map(px, 0, 128, 0, 9) ];
      writeInterval(clkDiv); // Display interval at bottom of the diplay (before the note text)

      IntervalNoteMillis = IntervalNoteMillisOutputArr[ map(px, 0, 128, 0, 9) ];

      Note_Duration = 0;
      
      if ( sustainNoteMode == 0  ) {
        Note_Duration = 10 + potAvalue; // Note duration in ms + Pot Value (10ms is minimal)
      }
      else {
        Note_Duration = Note_Duration + IntervalNoteMillis + potAvalue; // Note duration in ms + Pot Value + Values determined by interval
      }

//                        tft.fillRect(0, 100, 128, 8, BLACK);                                 
//                        tft.setCursor(0,100); tft.print(IntervalNoteMillis);
//                        tft.setCursor(30,100); tft.print(Note_Duration);
//                        tft.setCursor(60,100); tft.print(clkDiv);

      // Determine VELOCITY and display ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
      Note_Velocity = pz;  // MIDI Velocity 0 ... 127
      
      writeVelocity(Note_Velocity);

      if ( velocityMode == 0 ) { writeVelocity_FIXED(Note_Velocity); Note_Velocity = 127; }

      
      // Determine NOTE and display -------------------------------------------------------------------------------------------------------------------------------------------------------------------------
      fetchNote = ( map(py, 0, 128, midiSubSelectionNotesPointer, 0) ) - 1; // "midiSubSelectionNotesPointer" reflects the number of MIDI note numbers selected and stored in "midiSubSelectionNotesArr" 
      Note_Pitch = midiSubSelectionNotesArr[ fetchNote ];                                                                                                     

      writeNote(midiNotesTXTarr[Note_Pitch-12],110);

         switch (octTranspose) {
                  case -24:
                  if (Note_Pitch >= 31) { Note_Pitch = Note_Pitch - 24; } 
                  break;

                  case -23:
                  if (Note_Pitch >= 30) { Note_Pitch = Note_Pitch - 24; } 
                  break;

                  case -22:
                  if (Note_Pitch >= 29) { Note_Pitch = Note_Pitch - 22; } 
                  break;

                  case -21:
                  if (Note_Pitch >= 28) { Note_Pitch = Note_Pitch - 21; } 
                  break;

                  case -20:
                  if (Note_Pitch >= 27) { Note_Pitch = Note_Pitch - 20; } 
                  break;

                  case -19:
                  if (Note_Pitch >= 26) { Note_Pitch = Note_Pitch - 19; } 
                  break;

                  case -18:
                  if (Note_Pitch >= 25) { Note_Pitch = Note_Pitch - 18; } 
                  break;

                  case -17:
                  if (Note_Pitch >= 24) { Note_Pitch = Note_Pitch - 17; } 
                  break;
                  
                  case -16:
                  if (Note_Pitch >= 23) { Note_Pitch = Note_Pitch - 16; } 
                  break;

                  case -15:
                  if (Note_Pitch >= 22) { Note_Pitch = Note_Pitch - 15; } 
                  break;

                  case -14:
                  if (Note_Pitch >= 21) { Note_Pitch = Note_Pitch - 14; } 
                  break;

                  case -13:
                  if (Note_Pitch >= 20) { Note_Pitch = Note_Pitch - 13; } 
                  break;
                  
                  case -12:
                  if (Note_Pitch >= 19) { Note_Pitch = Note_Pitch - 12; }
                  break;

                  case -11:
                  if (Note_Pitch >= 18) { Note_Pitch = Note_Pitch - 11; } 
                  break;

                  case -10:
                  if (Note_Pitch >= 17) { Note_Pitch = Note_Pitch - 10; } 
                  break;

                  case -9:
                  if (Note_Pitch >= 16) { Note_Pitch = Note_Pitch - 9; } 
                  break;

                  case -8:
                  if (Note_Pitch >= 15) { Note_Pitch = Note_Pitch - 8; } 
                  break;

                  case -7:
                  if (Note_Pitch >= 14) { Note_Pitch = Note_Pitch - 7; } 
                  break;

                  case -6:
                  if (Note_Pitch >= 13) { Note_Pitch = Note_Pitch - 6; } 
                  break;

                  case -5:
                  if (Note_Pitch >= 12) { Note_Pitch = Note_Pitch - 5; } 
                  break;

                  case -4:
                  if (Note_Pitch >= 11) { Note_Pitch = Note_Pitch - 4; } 
                  break;
                  
                  case -3:
                  if (Note_Pitch >= 10) { Note_Pitch = Note_Pitch - 3; } 
                  break;

                  case -2:
                  if (Note_Pitch >= 9) { Note_Pitch = Note_Pitch - 2; } 
                  break;

                  case -1:
                  if (Note_Pitch >= 8) { Note_Pitch = Note_Pitch - 1; } 
                  break;

                  //   /\
                  //   || DOWN
                  // -------------------------------------------------------------------------------------
                  //   || UP
                  //   \/
                  
                  case 1:
                  if (Note_Pitch <= 126) { Note_Pitch = Note_Pitch + 1; }
                  break;                               
                  
                  case 2:
                  if (Note_Pitch <= 125) { Note_Pitch = Note_Pitch + 2; }
                  break;                  
                  
                  case 3:
                  if (Note_Pitch <= 124) { Note_Pitch = Note_Pitch + 3; }
                  break;                  
                  
                  case 4:
                  if (Note_Pitch <= 123) { Note_Pitch = Note_Pitch + 4; }
                  break;                  
                  
                  case 5:
                  if (Note_Pitch <= 122) { Note_Pitch = Note_Pitch + 5; }
                  break;                  
                  
                  case 6:
                  if (Note_Pitch <= 121) { Note_Pitch = Note_Pitch + 6; }
                  break;                  
                  
                  case 7:
                  if (Note_Pitch <= 120) { Note_Pitch = Note_Pitch + 7; }
                  break;                  
                  
                  case 8:
                  if (Note_Pitch <= 119) { Note_Pitch = Note_Pitch + 8; }
                  break;                  
                  
                  case 9:
                  if (Note_Pitch <= 118) { Note_Pitch = Note_Pitch + 9; }
                  break;                  
                  
                  case 10:
                  if (Note_Pitch <= 117) { Note_Pitch = Note_Pitch + 10; }
                  break;                  
                  
                  case 11:
                  if (Note_Pitch <= 116) { Note_Pitch = Note_Pitch + 11; }
                  break;                  
                  
                  case 12:
                  if (Note_Pitch <= 115) { Note_Pitch = Note_Pitch + 12; }
                  break;

                  case 13:
                  if (Note_Pitch <= 114) { Note_Pitch = Note_Pitch + 13; }
                  break;                               
                  
                  case 14:
                  if (Note_Pitch <= 113) { Note_Pitch = Note_Pitch + 14; }
                  break;                  
                  
                  case 15:
                  if (Note_Pitch <= 112) { Note_Pitch = Note_Pitch + 15; }
                  break;                  
                  
                  case 16:
                  if (Note_Pitch <= 111) { Note_Pitch = Note_Pitch + 16; }
                  break;                  
                  
                  case 17:
                  if (Note_Pitch <= 110) { Note_Pitch = Note_Pitch + 17; }
                  break;                  
                  
                  case 18:
                  if (Note_Pitch <= 109) { Note_Pitch = Note_Pitch + 18; }
                  break;                  
                  
                  case 19:
                  if (Note_Pitch <= 108) { Note_Pitch = Note_Pitch + 19; }
                  break;                  
                  
                  case 20:
                  if (Note_Pitch <= 107) { Note_Pitch = Note_Pitch + 20; }
                  break;                  
                  
                  case 21:
                  if (Note_Pitch <= 106) { Note_Pitch = Note_Pitch + 21; }
                  break;                  
                  
                  case 22:
                  if (Note_Pitch <= 105) { Note_Pitch = Note_Pitch + 22; }
                  break;                  
                  
                  case 23:
                  if (Note_Pitch <= 104) { Note_Pitch = Note_Pitch + 23; }
                  break;                  

                  case 24:
                  if (Note_Pitch <= 103) { Note_Pitch = Note_Pitch + 24; }
                  break;               
                  
         }

}

void drawTriDown(int lineYpos)
{

  tft.fillRect(123, lineYpos-9, 5, 8, BLACK);
  tft.fillTriangle(125, lineYpos-3, 123, lineYpos-7, 127, lineYpos-7, RED);
  
  tft.drawFastHLine(0, lineYpos-2, 128, BLACK);
  tft.drawFastHLine(0, lineYpos-1, 128, WHITE);
  tft.drawFastHLine(0, lineYpos, 128, BLACK);  
}

void drawTriUp(int lineYpos)
{
  
  tft.fillRect(123, lineYpos, 5, 8, BLACK);
  tft.fillTriangle(125, lineYpos+1, 123, lineYpos+5, 127, lineYpos+5, RED);

  tft.drawFastHLine(0, lineYpos, 128, BLACK);
  tft.drawFastHLine(0, lineYpos-1, 128, WHITE);
  tft.drawFastHLine(0, lineYpos-2, 128, BLACK);
  
}

void drawNotesGrid()
{

int notesGrid_x = 0;
int notesGrid_y = 20;

tft.setCursor(notesGrid_x+1,notesGrid_y-20); tft.print("Octave (y-axis)");
  tft.drawFastVLine(notesGrid_x+3,notesGrid_y-10, 14, GREY);

  tft.drawFastVLine(notesGrid_x+8,notesGrid_y-5, 103, GREY);

tft.fillRect(notesGrid_x+9, notesGrid_y-5, 9, 13, WHITE);
tft.setTextColor(BLACK);
tft.setCursor(notesGrid_x+11,notesGrid_y-3); tft.print("C");
  tft.drawFastVLine(notesGrid_x+18,notesGrid_y-5, 103, GREY);

tft.setTextColor(WHITE);
tft.setCursor(notesGrid_x+21,notesGrid_y-3); tft.print("C");
  tft.drawFastVLine(notesGrid_x+28,notesGrid_y-5, 103, GREY);

tft.fillRect(notesGrid_x+29, notesGrid_y-5, 9, 13, WHITE);
tft.setTextColor(BLACK);
tft.setCursor(notesGrid_x+31,notesGrid_y-3); tft.print("D");
  tft.drawFastVLine(notesGrid_x+38,notesGrid_y-5, 103, GREY);

tft.setTextColor(WHITE);
tft.setCursor(notesGrid_x+41,notesGrid_y-3); tft.print("D");
  tft.drawFastVLine(notesGrid_x+48,notesGrid_y-5, 103, GREY);

tft.fillRect(notesGrid_x+49, notesGrid_y-5, 9, 13, WHITE);
tft.setTextColor(BLACK);
tft.setCursor(notesGrid_x+51,notesGrid_y-3); tft.print("E");
  tft.drawFastVLine(notesGrid_x+58,notesGrid_y-5, 103, GREY);

tft.fillRect(notesGrid_x+59, notesGrid_y-5, 9, 13, WHITE);
tft.setCursor(notesGrid_x+61,notesGrid_y-3); tft.print("F");
  tft.drawFastVLine(notesGrid_x+68,notesGrid_y-5, 103, GREY);

tft.setTextColor(WHITE);
tft.setCursor(notesGrid_x+71,notesGrid_y-3); tft.print("F");
  tft.drawFastVLine(notesGrid_x+78,notesGrid_y-5, 103, GREY);

tft.fillRect(notesGrid_x+79, notesGrid_y-5, 9, 13, WHITE);
tft.setTextColor(BLACK);
tft.setCursor(notesGrid_x+81,notesGrid_y-3); tft.print("G");
  tft.drawFastVLine(notesGrid_x+88,notesGrid_y-5, 103, GREY);

tft.setTextColor(WHITE);
tft.setCursor(notesGrid_x+91,notesGrid_y-3); tft.print("G");
  tft.drawFastVLine(notesGrid_x+98,notesGrid_y-5, 103, GREY);

tft.fillRect(notesGrid_x+99, notesGrid_y-5, 9, 13, WHITE);
tft.setTextColor(BLACK);
tft.setCursor(notesGrid_x+101,notesGrid_y-3); tft.print("A");
  tft.drawFastVLine(notesGrid_x+108,notesGrid_y-5, 103, GREY);

tft.setTextColor(WHITE);
tft.setCursor(notesGrid_x+111,notesGrid_y-3); tft.print("A");
  tft.drawFastVLine(notesGrid_x+118,notesGrid_y-5, 103, GREY);

tft.fillRect(notesGrid_x+119, notesGrid_y-5, 9, 13, WHITE);
tft.setTextColor(BLACK);  
tft.setCursor(notesGrid_x+121,notesGrid_y-3); tft.print("B");

tft.drawFastHLine(notesGrid_x,notesGrid_y+8, 128, GREY);
tft.drawFastHLine(notesGrid_x,notesGrid_y+18, 128, GREY);
tft.drawFastHLine(notesGrid_x,notesGrid_y+28, 128, GREY);
tft.drawFastHLine(notesGrid_x,notesGrid_y+38, 128, GREY);
tft.drawFastHLine(notesGrid_x,notesGrid_y+48, 128, GREY);
tft.drawFastHLine(notesGrid_x,notesGrid_y+58, 128, GREY);
tft.drawFastHLine(notesGrid_x,notesGrid_y+68, 128, GREY);
tft.drawFastHLine(notesGrid_x,notesGrid_y+78, 128, GREY);
tft.drawFastHLine(notesGrid_x,notesGrid_y+88, 128, GREY);
tft.drawFastHLine(notesGrid_x,notesGrid_y+98, 128, GREY);

tft.drawRect(9, 29, 9, 9, WHITE); 

tft.setTextColor(WHITE);

tft.setCursor(notesGrid_x+1,notesGrid_y+10); tft.print("8");
tft.setCursor(notesGrid_x+1,notesGrid_y+20); tft.print("7");
tft.setCursor(notesGrid_x+1,notesGrid_y+30); tft.print("6");
tft.setCursor(notesGrid_x+1,notesGrid_y+40); tft.print("5");
tft.setCursor(notesGrid_x+1,notesGrid_y+50); tft.print("4");
tft.setCursor(notesGrid_x+1,notesGrid_y+60); tft.print("3");
tft.setCursor(notesGrid_x+1,notesGrid_y+70); tft.print("2");
tft.setCursor(notesGrid_x+1,notesGrid_y+80); tft.print("1");
tft.setCursor(notesGrid_x+1,notesGrid_y+90); tft.print("0");

drawGridSquaresNotes();
  
}

void drawGridSquaresNotes() {

    int EnaNotesX = 10;
    int EnaNotesY = 30;
    int EnaDisNotesArrPointer = 0;
   

   if ( notesShuffled == 0 && snapShotRetrieved == 0 ) { // When notes are shuffled (=1), only update the array if we add or delete a note from screen otherwise the notes are re-ordered when switching back to mode 1

          for( int i = 0; i < 108;  ++i ) { midiSubSelectionNotesArr[i] = (char)0; } // Clear midiSubSelectionNotesArr for re-population
              
   }
   
    midiSubSelectionNotesPointer = 0;
    
    for (int row=0; row <= 8; row++) {
    
           for (int col=0; col <= 11; col++) { 
    
              if ( EnaDisNotesArr[EnaDisNotesArrPointer] == 1 ) {
                
                if ( operationMode == 1 ) { tft.fillRect(EnaNotesX, EnaNotesY, 7, 7, RED); }

                  if ( notesShuffled == 0 && snapShotRetrieved == 0 ) { // When notes are shuffled (=1), only update the array if we add or delete a note from screen otherwise the notes are re-ordered when switching back to mode 1
                  
                       midiSubSelectionNotesArr[midiSubSelectionNotesPointer] = midiNotesArr[EnaDisNotesArrPointer];

                    }
                                         
                       midiSubSelectionNotesPointer++; // Once the grid squares have been drawn, "midiSubSelectionNotesPointer" reflects the number of elements in the "midiSubSelectionNotesArr" array 
                  
              }
              else {
  
                if ( operationMode == 1 ) { tft.fillRect(EnaNotesX, EnaNotesY, 7, 7, BLACK); }

                   if ( notesShuffled == 0 && snapShotRetrieved == 0 ) { // When notes are shuffled (=1), only update the array if we add or delete a note from screen otherwise the notes are re-ordered when switching back to mode 1   
                      
                      if ( row == 8 && col == 11 && midiSubSelectionNotesPointer == 0 ) { for( int i = 0; i < 108;  ++i ) { midiSubSelectionNotesArr[i] = (char)0; }  } // No notes selected at all = Clear midiSubSelectionNotesArr once more 

                   }
                
              }
    
              EnaNotesX = EnaNotesX + 10;

              if ( notesShuffled == 0 && snapShotRetrieved == 0 ) { // When notes are shuffled (=1), only update the array if we add or delete a note from screen otherwise the notes are re-ordered when switching back to mode 1 
                  EEPROM.write(EnaDisNotesArrPointer, EnaDisNotesArr[EnaDisNotesArrPointer]);              
                  EEPROM.commit();
              }
              
              EnaDisNotesArrPointer++;    
              
           }
    
      EnaNotesX = 10;
      EnaNotesY = EnaNotesY + 10;
     
    
    }


    if ( notesShuffled == 0 && snapShotRetrieved == 0 ) {
      qsort(midiSubSelectionNotesArr, midiSubSelectionNotesPointer, sizeof(int), cmpfunc); // Sort midiSubSelectionNotesArr from low to high 
    }

    getActualNumberOfNotes();
 
}



void drawIntervalGrid() {

int intervalGrid_x = 0;
int intervalGrid_y = 20;

tft.setCursor(intervalGrid_x+1,intervalGrid_y-20); tft.print("Interval (x-axis)");
  tft.drawFastVLine(intervalGrid_x+3,intervalGrid_y-10, 14, GREY);

tft.drawFastVLine(intervalGrid_x+28,intervalGrid_y+8, 90, GREY);
tft.drawFastVLine(intervalGrid_x+38,intervalGrid_y+8, 90, GREY);
tft.drawFastVLine(intervalGrid_x+48,intervalGrid_y+8, 90, GREY);
tft.drawFastVLine(intervalGrid_x+58,intervalGrid_y+8, 90, GREY);
tft.drawFastVLine(intervalGrid_x+68,intervalGrid_y+8, 90, GREY);
tft.drawFastVLine(intervalGrid_x+78,intervalGrid_y+8, 90, GREY);
tft.drawFastVLine(intervalGrid_x+88,intervalGrid_y+8, 90, GREY);
tft.drawFastVLine(intervalGrid_x+98,intervalGrid_y+8, 90, GREY);
tft.drawFastVLine(intervalGrid_x+108,intervalGrid_y+8, 90, GREY);
tft.drawFastVLine(intervalGrid_x+118,intervalGrid_y+8, 90, GREY);

tft.drawFastHLine(intervalGrid_x,intervalGrid_y+8, 128, GREY);
tft.drawFastHLine(intervalGrid_x,intervalGrid_y+18, 128, GREY);
tft.drawFastHLine(intervalGrid_x,intervalGrid_y+28, 128, GREY);
tft.drawFastHLine(intervalGrid_x,intervalGrid_y+38, 128, GREY);
tft.drawFastHLine(intervalGrid_x,intervalGrid_y+48, 128, GREY);
tft.drawFastHLine(intervalGrid_x,intervalGrid_y+58, 128, GREY);
tft.drawFastHLine(intervalGrid_x,intervalGrid_y+68, 128, GREY);
tft.drawFastHLine(intervalGrid_x,intervalGrid_y+78, 128, GREY);
tft.drawFastHLine(intervalGrid_x,intervalGrid_y+88, 128, GREY);
tft.drawFastHLine(intervalGrid_x,intervalGrid_y+98, 128, GREY);

tft.drawRect(29, 29, 9, 9, WHITE); 

tft.setTextColor(GREEN);
tft.setCursor(intervalGrid_x+1,intervalGrid_y+10); tft.print("1/16");
tft.setTextColor(ORANGE);
tft.setCursor(intervalGrid_x+1,intervalGrid_y+20); tft.print("1/12");
tft.setTextColor(GREEN);
tft.setCursor(intervalGrid_x+1,intervalGrid_y+30); tft.print("1/8");
tft.setTextColor(ORANGE);
tft.setCursor(intervalGrid_x+1,intervalGrid_y+40); tft.print("1/6");
tft.setTextColor(GREEN);
tft.setCursor(intervalGrid_x+1,intervalGrid_y+50); tft.print("1/4");
tft.setTextColor(ORANGE);
tft.setCursor(intervalGrid_x+1,intervalGrid_y+60); tft.print("1/3");
tft.setTextColor(GREEN);
tft.setCursor(intervalGrid_x+1,intervalGrid_y+70); tft.print("1/2");
tft.setTextColor(ORANGE);
tft.setCursor(intervalGrid_x+1,intervalGrid_y+80); tft.print("3/4");
tft.setTextColor(GREEN);
tft.setCursor(intervalGrid_x+1,intervalGrid_y+90); tft.print("1");
tft.setTextColor(WHITE);

drawGridSquaresInterval();
  
}


void drawGridSquaresInterval() {

    int EnaIntervalX = 30;
    int EnaIntervalY = 30;
    int EnaDisIntervalArrPointer = 0;
    int AlternateColour = 0;

      if ( snapShotRetrieved == 0 ) {
        for( int i = 0; i < 10;  ++i ) { midiSubSelectionIntervalArr[i] = (char)0; } // Clear midiSubSelectionIntervalArr for re-population
      }

    midiSubSelectionIntervalPointer = 0;

      for (int row=0; row <= 8; row++) {
      
             for (int col=0; col <= 9; col++) {

                if ( EnaDisIntervalArr[EnaDisIntervalArrPointer] == 1 ) {
                
                  if ( operationMode == 2 ) { 
                    
                      if (AlternateColour % 2 == 0) {
                        tft.fillRect(EnaIntervalX, EnaIntervalY, 7, 7, GREEN);  
                      } else {
                        tft.fillRect(EnaIntervalX, EnaIntervalY, 7, 7, ORANGE);  
                      }
                      
                    
                    }

                      if ( snapShotRetrieved == 0 ) {
                        
                          // What Clock Divider is referred to? Drop the right divider in the midiSubSelectionIntervalArr (the interval menu does not include 1/23 and 1/32 - so, we start with clkDivDefaultsArr[2] = 1/16 )
                          if ( EnaDisIntervalArrPointer >= 0 && EnaDisIntervalArrPointer <= 9 ) { intervalDiv = clkDivDefaultsArr[2]; IntervalNoteMillis = IntervalNoteMillisArr[0]; }     // 16th
                          if ( EnaDisIntervalArrPointer >= 10 && EnaDisIntervalArrPointer <= 19 ) { intervalDiv = clkDivDefaultsArr[3]; IntervalNoteMillis = IntervalNoteMillisArr[1]; }   // 12th
                          if ( EnaDisIntervalArrPointer >= 20 && EnaDisIntervalArrPointer <= 29 ) { intervalDiv = clkDivDefaultsArr[4]; IntervalNoteMillis = IntervalNoteMillisArr[2]; }   // 8th
                          if ( EnaDisIntervalArrPointer >= 30 && EnaDisIntervalArrPointer <= 39 ) { intervalDiv = clkDivDefaultsArr[5]; IntervalNoteMillis = IntervalNoteMillisArr[3]; }   // 6th
                          if ( EnaDisIntervalArrPointer >= 40 && EnaDisIntervalArrPointer <= 49 ) { intervalDiv = clkDivDefaultsArr[6]; IntervalNoteMillis = IntervalNoteMillisArr[4]; }   // 4th
                          if ( EnaDisIntervalArrPointer >= 50 && EnaDisIntervalArrPointer <= 59 ) { intervalDiv = clkDivDefaultsArr[7]; IntervalNoteMillis = IntervalNoteMillisArr[5]; }   // 3rd
                          if ( EnaDisIntervalArrPointer >= 60 && EnaDisIntervalArrPointer <= 69 ) { intervalDiv = clkDivDefaultsArr[8]; IntervalNoteMillis = IntervalNoteMillisArr[6]; }   // 2nd
                          if ( EnaDisIntervalArrPointer >= 70 && EnaDisIntervalArrPointer <= 79 ) { intervalDiv = clkDivDefaultsArr[9]; IntervalNoteMillis = IntervalNoteMillisArr[7]; }   // Quarter
                          if ( EnaDisIntervalArrPointer >= 80 && EnaDisIntervalArrPointer <= 89 ) { intervalDiv = clkDivDefaultsArr[10]; IntervalNoteMillis = IntervalNoteMillisArr[8]; }  // Whole
                          
                          midiSubSelectionIntervalArr[col] = intervalDiv;
                          IntervalNoteMillisOutputArr[col] = IntervalNoteMillis;
                            
                      }

                      

                      midiSubSelectionIntervalPointer++;
                 
                }
                else {

                  if ( operationMode == 2 ) { tft.fillRect(EnaIntervalX, EnaIntervalY, 7, 7, BLACK); }
                  
                }

             

             EnaIntervalX = EnaIntervalX + 10;
             EnaDisIntervalArrPointer++; 
              
             }

       EnaIntervalX = 30;
       EnaIntervalY = EnaIntervalY + 10; 
       AlternateColour++;   

        
         

      }

}

void writeInterval(int intervalNr) {

  for (int interval=0; interval <= 10; interval++) {

      if ( clkDivDefaultsArr[interval] == intervalNr ) {
  
        clrBottomTextInterval();
        tft.setCursor(78,121); tft.print(clkDivDefaultsTXTarr[interval]);
         
      }
    
  }
  
}

void clrBottomTextInterval()
  {
    tft.fillRect(78, 121, 32, 7, BLACK);
  }

void writeVelocity(int VelOfNote) {

      clrBottomVelocity();
      tft.setCursor(55,121); tft.print(VelOfNote);
  
}


void writeVelocity_FIXED(int VelOfNote) {

      clrBottomVelocity();
      tft.setTextColor(PINK);
      tft.setCursor(55,121); tft.print(VelOfNote);
      tft.setTextColor(WHITE);
  
}

void clrBottomVelocity()
  {
    tft.fillRect(55, 121, 23, 7, BLACK);
  }

void storeShuffledinterval()
  {

      int intervalOffset = 0;

      for( int iInterval = 0; iInterval <= 9;  iInterval++ ) {

        // run through shuffled array
        newInterval = midiSubSelectionIntervalArr[iInterval];
         
            if ( newInterval == 6 ) { gridNumber =   0 + intervalOffset; }
            if ( newInterval == 8 ) { gridNumber =  10 + intervalOffset; } 
            if ( newInterval == 12 ) { gridNumber = 20 + intervalOffset; } 
            if ( newInterval == 16 ) { gridNumber = 30 + intervalOffset; } 
            if ( newInterval == 24 ) { gridNumber = 40 + intervalOffset; }                         
            if ( newInterval == 32 ) { gridNumber = 50 + intervalOffset; } 
            if ( newInterval == 48 ) { gridNumber = 60 + intervalOffset; } 
            if ( newInterval == 72 ) { gridNumber = 70 + intervalOffset; } 
            if ( newInterval == 96 ) { gridNumber = 80 + intervalOffset; } 
   

                    // Check in what column the change is made then disable current interval and enable new interval
                
                    // Column 1 START ===============================================
                    for( int i = 0; i <= 80;  i=i+10 ) {
                    
                      if ( i == gridNumber ) { 
                        
                            // Disable current selection  
                            for( int q = 108; q < 198;  q=q+10 ) {
                              if ( EEPROM.read(q) == 1 ) {
                                EEPROM.write(q, 0);
                                EEPROM.commit();
                                EnaDisIntervalArr[q-108] = 0;    
                              }
                            }

                      // Enable current selection
                      EEPROM.write(gridNumber+108, 1);
                      EEPROM.commit();
                      }
                      
                    }                                      
                    // Column 1 END =================================================

                      // Column 2 START ===============================================
                      for( int i = 1; i <= 81;  i=i+10 ) {
                      
                        if ( i == gridNumber ) { 
                          
                              // Disable current selection  
                              for( int q = 109; q < 198;  q=q+10 ) {
                                if ( EEPROM.read(q) == 1 ) {
                                  EEPROM.write(q, 0);
                                  EEPROM.commit();
                                  EnaDisIntervalArr[q-108] = 0;    
                                }
                              }

                        // Enable current selection
                        EEPROM.write(gridNumber+108, 1);
                        EEPROM.commit();
                        }
                        
                      }
                      // Column 2 END =================================================

                        // Column 3 START ===============================================
                        for( int i = 2; i <= 82;  i=i+10 ) {
                        
                          if ( i == gridNumber ) { 
                            
                                // Disable current selection  
                                for( int q = 110; q < 198;  q=q+10 ) {
                                  if ( EEPROM.read(q) == 1 ) {
                                    EEPROM.write(q, 0);
                                    EEPROM.commit();
                                    EnaDisIntervalArr[q-108] = 0;    
                                  }
                                }

                          // Enable current selection
                          EEPROM.write(gridNumber+108, 1);
                          EEPROM.commit();
                          }
                          
                        }
                        // Column 3 END =================================================

                          // Column 4 START ===============================================
                          for( int i = 3; i <= 83;  i=i+10 ) {
                          
                            if ( i == gridNumber ) { 
                              
                                  // Disable current selection  
                                  for( int q = 111; q < 198;  q=q+10 ) {
                                    if ( EEPROM.read(q) == 1 ) {
                                      EEPROM.write(q, 0);
                                      EEPROM.commit();
                                      EnaDisIntervalArr[q-108] = 0;    
                                    }
                                  }

                            // Enable current selection
                            EEPROM.write(gridNumber+108, 1);
                            EEPROM.commit();
                            }
                            
                          }
                          // Column 4 END =================================================

                            // Column 5 START ===============================================
                            for( int i = 4; i <= 84;  i=i+10 ) {
                            
                              if ( i == gridNumber ) { 
                                
                                    // Disable current selection  
                                    for( int q = 112; q < 198;  q=q+10 ) {
                                      if ( EEPROM.read(q) == 1 ) {
                                        EEPROM.write(q, 0);
                                        EEPROM.commit();
                                        EnaDisIntervalArr[q-108] = 0;    
                                      }
                                    }

                              // Enable current selection
                              EEPROM.write(gridNumber+108, 1);
                              EEPROM.commit();
                              }
                              
                            }
                            // Column 5 END =================================================

                              // Column 6 START ===============================================
                              for( int i = 5; i <= 85;  i=i+10 ) {
                              
                                if ( i == gridNumber ) { 
                                  
                                      // Disable current selection  
                                      for( int q = 113; q < 198;  q=q+10 ) {
                                        if ( EEPROM.read(q) == 1 ) {
                                          EEPROM.write(q, 0);
                                          EEPROM.commit();
                                          EnaDisIntervalArr[q-108] = 0;    
                                        }
                                      }

                                // Enable current selection
                                EEPROM.write(gridNumber+108, 1);
                                EEPROM.commit();
                                }
                                
                              }
                              // Column 6 END =================================================
                            

                                // Column 7 START ===============================================
                                for( int i = 6; i <= 86;  i=i+10 ) {
                                
                                  if ( i == gridNumber ) { 
                                    
                                        // Disable current selection  
                                        for( int q = 114; q < 198;  q=q+10 ) {
                                          if ( EEPROM.read(q) == 1 ) {
                                            EEPROM.write(q, 0);
                                            EEPROM.commit();
                                            EnaDisIntervalArr[q-108] = 0;    
                                          }
                                        }

                                  // Enable current selection
                                  EEPROM.write(gridNumber+108, 1);
                                  EEPROM.commit();
                                  }
                                  
                                }
                                // Column 7 END =================================================

                                  // Column 8 START ===============================================
                                  for( int i = 7; i <= 87;  i=i+10 ) {
                                  
                                    if ( i == gridNumber ) { 
                                      
                                          // Disable current selection  
                                          for( int q = 115; q < 198;  q=q+10 ) {
                                            if ( EEPROM.read(q) == 1 ) {
                                              EEPROM.write(q, 0);
                                              EEPROM.commit();
                                              EnaDisIntervalArr[q-108] = 0;    
                                            }
                                          }

                                    // Enable current selection
                                    EEPROM.write(gridNumber+108, 1);
                                    EEPROM.commit();
                                    }
                                    
                                  }
                                  // Column 8 END =================================================

                                    // Column 9 START ===============================================
                                    for( int i = 8; i <= 88;  i=i+10 ) {
                                    
                                      if ( i == gridNumber ) { 
                                        
                                            // Disable current selection  
                                            for( int q = 116; q < 198;  q=q+10 ) {
                                              if ( EEPROM.read(q) == 1 ) {
                                                EEPROM.write(q, 0);
                                                EEPROM.commit();
                                                EnaDisIntervalArr[q-108] = 0;    
                                              }
                                            }
  
                                      // Enable current selection
                                      EEPROM.write(gridNumber+108, 1);
                                      EEPROM.commit();
                                      }
                                      
                                    }
                                    // Column 9 END =================================================

                                      // Column 10 START ===============================================
                                      for( int i = 9; i <= 89;  i=i+10 ) {
                                      
                                        if ( i == gridNumber ) { 
                                          
                                              // Disable current selection  
                                              for( int q = 117; q < 198;  q=q+10 ) {
                                                if ( EEPROM.read(q) == 1 ) {
                                                  EEPROM.write(q, 0);
                                                  EEPROM.commit();
                                                  EnaDisIntervalArr[q-108] = 0;    
                                                }
                                              }
    
                                        // Enable current selection
                                        EEPROM.write(gridNumber+108, 1);
                                        EEPROM.commit();
                                        }
                                        
                                      }
                                      // Column 10 END =================================================                              

      
      intervalOffset++; 
      }

        for( int i = 108; i < 198;  ++i ) {
         
        EnaDisIntervalArr[i-108] = EEPROM.read(i);
        }

  drawGridSquaresInterval(); 
  }



void drawPlayAnimation(int step0_3) {

tft.fillCircle(5, 124, 3, DARKGREY); 
tft.fillCircle(13, 124, 3, DARKGREY); 
tft.fillCircle(21, 124, 3, DARKGREY); 
tft.fillCircle(29, 124, 3, DARKGREY); 
  
  switch (step0_3) {
    case 0:
      tft.fillCircle(5, 124, 3, RED);  
    break;
    case 1:
      tft.fillCircle(13, 124, 3, LIGHTBLUE);  
    break;    
    case 2:
      tft.fillCircle(21, 124, 3, LIGHTBLUE);  
    break;
    case 3:
      tft.fillCircle(29, 124, 3, LIGHTBLUE);
    break;      
  }
  
}


void getActualNumberOfNotes() {

    if ( snapShotRetrieved == 1 ) {

      if ( retrievedPattern == 1 ) { midiSubSelectionNotesPointer = midiSubSelectionNotesPointerSnap_1; }

      if ( retrievedPattern == 2 ) { midiSubSelectionNotesPointer = midiSubSelectionNotesPointerSnap_2; }
      
    }
  
}

void drawCube() {

  tft.setTextSize(2);
  tft.setCursor(5,80); 
  tft.setTextColor(YELLOW);
  tft.print("DIMENSIONS");  
  tft.setTextSize(1);
  tft.setCursor(5,100);
  tft.print("Release 1.0");
  tft.setCursor(5,120);
  tft.print("erikoostveen.co.uk");

    pointShift = 0;
    CubeDef(pointShift);

    delay(1000);

    for( int repeat = 0; repeat < 2;  repeat++ ) {
        
        for( int pointShift = 0; pointShift < 10;  pointShift++ ) {
          CubeDef(pointShift);
          delay(80);
          tft.fillRect(1,1, 128, 75, BLACK);
        }    
    
        for( int pointShift = 10; pointShift > 0;  pointShift-- ) {
          CubeDef(pointShift);
          delay(80);
          tft.fillRect(1,1, 128, 75, BLACK);
        }

    }

    CubeDef(pointShift);

    delay(2000);

    clrScreen();
      
}

void CubeDef(int pointShift) {

  int p1x = xCube-aCube + pointShift;        int p1y = yCube-aCube + pointShift;
  int p2x = xCube+aCube - pointShift;        int p2y = yCube-aCube + pointShift;
  int p3x = xCube+aCube - pointShift;        int p3y = yCube+aCube - pointShift;
  int p4x = xCube-aCube + pointShift;        int p4y = yCube+aCube - pointShift;
  int p5x = xCube-aCube+bCube + pointShift;  int p5y = yCube-aCube-bCube + pointShift;
  int p6x = xCube+aCube+bCube - pointShift;  int p6y = yCube-aCube-bCube + pointShift;
  int p7x = xCube+aCube+bCube - pointShift;  int p7y = yCube+aCube-bCube - pointShift;
  int p8x = xCube-aCube+bCube + pointShift;  int p8y = yCube+aCube-bCube - pointShift;

  tft.drawLine(p5x,p5y,p6x,p6y, YELLOW);
  tft.drawLine(p6x,p6y,p7x,p7y, YELLOW);
  tft.drawLine(p7x,p7y,p8x,p8y, YELLOW);
  tft.drawLine(p8x,p8y,p5x,p5y, YELLOW); 

  tft.drawLine(p1x,p1y,p5x,p5y, YELLOW);
  tft.drawLine(p2x,p2y,p6x,p6y, YELLOW);
  tft.drawLine(p3x,p3y,p7x,p7y, YELLOW);
  tft.drawLine(p4x,p4y,p8x,p8y, YELLOW);

  tft.drawLine(p1x,p1y,p2x,p2y, YELLOW);
  tft.drawLine(p2x,p2y,p3x,p3y, YELLOW);
  tft.drawLine(p3x,p3y,p4x,p4y, YELLOW);
  tft.drawLine(p4x,p4y,p1x,p1y, YELLOW);
                        
}

void showMidiClockmessage() {

  if ( midiTimeOutBlankScreen == 0 ) { clrScreen(); }

  tft.setTextColor(WHITE);
  
  tft.setCursor(25, 40);     
  tft.println("Waiting for");
  tft.setCursor(25, 50);     
  tft.println("MIDI clock...");
  
  midiTimeOutBlankScreen = 1;
  
}
