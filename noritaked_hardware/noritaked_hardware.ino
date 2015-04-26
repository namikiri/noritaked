#include <CUU_Interface.h>
#include <CUU_Parallel_M68.h>
#include <Noritake_VFD_CUU.h>

#define TRACK_FREEZE_TIME 3000
#define TRACK_MARQUEE_DELAY 50

#define SER_TRACK_START 0x01
#define SER_TRACK_STOP 0x02
#define SER_MISC_START 0x03
#define SER_MISC_STOP 0x04

#define SIG_INIT_OK 0x01
#define SIG_TRACK_UPDATED 0x02
#define SIG_MISC_UPDATED 0x03
#define SIG_BAD_SEQUENCE 0x04
#define SIG_LEN_EXCEEDED 0x05

#define MAX_TRACK_LEN 256
#define MAX_MISC_LEN 16





CUU_Parallel_M68_4bit interface(9, 10, 11, 4, 5, 6, 7); //RS,WR,RD,D4-D7
Noritake_VFD_CUU vfd;


void setup() {
  // VFD INITIALIZATION CODE
  delay (500);      // wait for device to power up
  vfd.begin(16, 2);    // 20x2 character module
  vfd.interface(interface); // select which interface to use
  vfd.CUU_init();      // initialize module
  vfd.CUU_noAutoscroll();
  vfd.CUU_clearScreen();

  // Just a nice startup
  String 
         welcome = "Starting up...",
         daemonv = "NTDH client v0.2";

  for (int i = 0; i < welcome.length(); i++)
  {
    vfd.print(welcome[i]);
    if (i > 16)
      vfd.CUU_scrollDisplayLeft();

    delay (50);
  }

  vfd.CUU_setCursor(0, 1);
  vfd.print(daemonv.c_str());
  delay (3000);
  
  vfd.CUU_clearScreen();

  Serial.begin(9600);
  Serial.write(SIG_INIT_OK);

}


int currentOffset = 0,
    totalOffsetAvailable = 0;
    
unsigned long lastTime = 0;

short int systemStates = B01000000;
/*
  Short int as array of booleans.
  Use bitwise AND to check them.
  
  1st bit [x0000000] : Freeze text at some position?
  2nd bit [0x000000] : Is direction left?
  3rd bit [00x00000] : We're buffering TRACK info now
  4th bit [000x0000] : We're buffering MISC info now
  5th bit [0000x000] : Unused
  6th bit [00000x00] : Unused
  7th bit [000000x0] : Unused
  8th bit [0000000x] : Unused
*/



String track = "Waiting for NIDS daemon connection",
       serialBuffer = "", lastTrack = "";

void loop() {
  // Serial processing code here
  // Serial buffer size
  int buffAvailable = Serial.available();
  // If something is received...
  if (buffAvailable)
  {
    for (int i = 0; i < buffAvailable; i++)
     {
       int ch = Serial.read();
       
       switch (ch)
       {
         case SER_TRACK_START :
           if (systemStates & B00100000 || systemStates & B00010000)
           {
              Serial.write (SIG_BAD_SEQUENCE);
              break;
           }
           // We're starting to buffer track info.
           systemStates = systemStates | B00100000;
           break;
           
         case SER_TRACK_STOP :
           if (systemStates & B00100000)
           {
             systemStates = systemStates ^ B00100000;
             if (serialBuffer.length() > MAX_TRACK_LEN)
             {
               Serial.write (SIG_LEN_EXCEEDED);
               serialBuffer = serialBuffer.substring(0, MAX_TRACK_LEN);  
             }
             track = serialBuffer;
             serialBuffer = "";
             Serial.write (SIG_TRACK_UPDATED);
           }
             else
               Serial.write (SIG_BAD_SEQUENCE); 
           break;
           
         case SER_MISC_START :
           if (systemStates & B00010000 || systemStates & B00100000)
           {
             Serial.write (SIG_BAD_SEQUENCE);
             break; 
           }
           // We're starting to buffer misc info.
           systemStates = systemStates | B00010000;
           break;
           
         case SER_MISC_STOP :
           if (systemStates & B00010000)
           {
             // Unsetting the flag
             systemStates = systemStates ^ B00010000;
             
             if (serialBuffer.length() > MAX_MISC_LEN)
             {
               Serial.write (SIG_LEN_EXCEEDED);
               serialBuffer = serialBuffer.substring (0, MAX_MISC_LEN);  
             }
             
             vfd.CUU_setCursor (0, 1);
             vfd.print("                "); // Noritake can't clear certain line. We'll use 16 spaces.
             vfd.CUU_setCursor (0, 1);
             vfd.print(serialBuffer.c_str());
             Serial.write (SIG_MISC_UPDATED);
             serialBuffer = "";
           }
             else
               Serial.write (SIG_BAD_SEQUENCE); 
               
           break;
           
         default :
           serialBuffer += char(ch);
       }       
     }
  }
  
  
  if (lastTrack != track)
  {
    lastTrack = track;
    totalOffsetAvailable = track.length() - 16;
    currentOffset = 0;
    // Show initial track and freeze it for TRACK_FREEZE_TIME
    track.substring(currentOffset, currentOffset + 16);
    vfd.CUU_setCursor(0, 0);
    vfd.print("                "); // Noritake can't clear certain line. We'll use 16 spaces.
    vfd.CUU_setCursor(0, 0);
    vfd.print(track.substring(currentOffset, currentOffset + 16).c_str());
  }
  
  
  
  if (track.length() <= 16)
  {
    if (lastTrack == track)
      return;
      else
    {
      vfd.print(track.c_str());
      return;
    }
  }
  else
  {
    if (systemStates & B10000000)
    {
        if (millis() - lastTime < TRACK_FREEZE_TIME)
        {
          return;
        }
          else
        {
          // Disable delay
          systemStates = systemStates ^ B10000000;
        }
    }
      else // else we'll pass milliseconds of marquee
    {
      if (millis() - lastTime < TRACK_MARQUEE_DELAY)
      {
        return;  
      }
      else
      {
        lastTime = millis();  
      }
    }
    
    
    String currentTrackSS;  // SS is for SubString, you little nazi!

    if (systemStates & B01000000)
    {
      if (currentOffset > totalOffsetAvailable)
      {
        systemStates = systemStates ^ B01000000;
        currentOffset--;
        
        // Enable delay
        lastTime = millis();
        systemStates = systemStates | B10000000;
      }
      else
      {
        currentTrackSS = track.substring(currentOffset, currentOffset + 16);
        currentOffset++;
      }
    }
    else
    {
      if (currentOffset < 0)
      {
        systemStates = systemStates | B01000000;
        currentOffset++;
        
        // Enable delay
        lastTime = millis();
        systemStates = systemStates | B10000000;
      }
      else
      {
        currentTrackSS = track.substring(currentOffset, currentOffset + 16);
        currentOffset--;
      }
    }

    delay (100);
    vfd.CUU_setCursor(0, 0);
    vfd.print(currentTrackSS.c_str());
  }


}
