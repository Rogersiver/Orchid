#include <Bounce2.h>
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// bounce time in ms
int bounceInterval = 200;                                                     //this is the time delay on each sensor trigger
// debouncer for each sensor pin used
Bounce bouncer[30] = Bounce();

// Pin of first sensor
int pinStart = 2;

// Total # Of sensor pins
int pinCount = 30;
//  6 of them are easter eggs
int eEggCount = 6;
//  That leaves us with 24 pins
int dualCount = (pinCount - eEggCount);
//  Pairing them up we have 12
int multiCount = (dualCount / 2);

//  storage vars for button states
int buttonState[30];
int lastButtonState[30];
//  Storage vars for multiState
int lastMultiState[12];
int multiState[12];

//  MIDI CHANNEL DEFS
//  MIDI channel for Lighting Signals
int litChan = 1;
//  MIDI Channel for paired sensor signals
int multiChan = 2;
// MIDI Channel for audio signals
int audChan = 3;
//  MIDI Channel for easter Eggs
int eEggChan = 4;

//  MIDI START NOTES
//  Easter Egg Start MIDI Note
int eEgg = 100;
//  Start MIDI note for lighting signals
int litStart = 60;
//  Start MIDI note for audio signals
int audStart = 40;

//  Counter var for how many people are sitting down
int counter = 0;
int lastCounter = 0;
//AUDIO DROP DOWN TIMER INTERVAL
int counterInterval = 2000;                                                    //this is for time it takes for audio to drop back down after triggering

//  COUNTER THRESHOLDS FOR ADDING MORE AUDIO LAYERS
int layer[] = {2, 4, 8, 12, 18, 24}; // 24 is for party mode
const int layerCount = sizeof(layer) / sizeof(int);
int trend = 0;

long layerTimer[layerCount];
int deferredAudMidiNote[layerCount - 1];

long lastRandomize = millis();
long randomizeInterval = 18000000;  //12000 debug time                             //this is the time it takes for the tracks to be shuffled
int randomOffset[] = {0, 1, 2, 3, 4};
bool multiVal[12] = {false};
bool randomVar = false;
bool control = false;
bool eggOn[5];                           //check initial egg sensor state

bool partyOn = false;
long partyTimer = millis();
long partyInterval = 60000;

bool sentStart = false;
           //change to (300000)

void setup()
{
  // Begin MIDI and listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  for (int i = 0; i < pinCount; i++)
  {
    pinMode(i + pinStart, INPUT);
    bouncer[i].attach(i + pinStart, INPUT);
    bouncer[i].interval(bounceInterval);
    buttonState[i] = bouncer[i].read();
    if ((i < dualCount) && (buttonState[i] == HIGH))
    {
      counter++;
    }
  }
}

void loop()
{
  if(sentStart == false){
    if(millis() - 0 >= (1000 * 60 * 3)){
      MIDI.sendNoteOn(); // value
      sentStart = true;
    }
  }

  bouncer[pinCount - 1].update();
  if (bouncer[pinCount - 1].changed())
  {
    buttonState[pinCount - 1] = bouncer[pinCount - 1].read();
    if (buttonState[pinCount - 1] == HIGH)
    {
      control = true;
    }
    else
    {
      control = false;
    }
  }

  for (int i = 0; i < pinCount - 1; i++)
  {
    bouncer[i].update();


    if (bouncer[i].changed())
    {
      buttonState[i] = bouncer[i].read();
      // Pin pairing logic (Quantum Enganglement)
      if (i < multiCount) {
        bouncer[i + multiCount].update();
        buttonState[i + multiCount] = bouncer[i + multiCount].read();
        if ((buttonState[i + multiCount] == HIGH) && (buttonState[i] == HIGH))
        {
          // sendNoteX(note, velocity, midiChannel)
          MIDI.sendNoteOn(i + litStart, 120, multiChan);
          multiVal[i] = true;
        }
        else if (multiVal[i])
        {
          MIDI.sendNoteOff(i + litStart, 0, multiChan);
          multiVal[i] = false;
        }
      }

      // If somebody sits down on any sensor thats non easter egg
      // Then send lighting midi note
      if ((i < dualCount) && (buttonState[i] == HIGH))
      {
        MIDI.sendNoteOn(i + litStart, 120, litChan);
        counter++;
      }
      else if ((i < dualCount) && (buttonState[i] != HIGH))
      {
        MIDI.sendNoteOff(i + litStart, 0, litChan);
        if (counter != 0) {
          counter--;
        }
      }

      // Easter egg ON trigger
      if (i >= dualCount)
      {
        buttonState[pinCount - 1] = bouncer[pinCount - 1].read();

        if (control && buttonState[i] == HIGH && i != 27)
        {
          eggOn [i] = true;
          // add isOn variable for each easter egg so we can check if its on to stop spamming off messages
          MIDI.sendNoteOn(i + eEgg - dualCount, 120, eEggChan);
          MIDI.sendNoteOff(i + eEgg - dualCount, 0, eEggChan);

        }
        else if (control && buttonState[i] == HIGH && i == 27)
        {
          eggOn [i] = true;
          // add isOn variable for each easter egg so we can check if its on to stop spamming off messages
          MIDI.sendNoteOn(layerCount - 1 +  audStart, 120, audChan);
          MIDI.sendNoteOff(layerCount - 1 + audStart, 0, audChan);

        }

        // Easter egg OFF trigger
        else if ((buttonState[i] != HIGH || !control) && eggOn[i] && i != 27) //only send this if isOn variable is on
        {
          eggOn [i] = false;
          MIDI.sendNoteOn(i + eEgg - dualCount + eEggCount, 120, eEggChan);
          MIDI.sendNoteOff(i + eEgg - dualCount + eEggCount, 0, eEggChan);
        }
        else if (((buttonState[i] != HIGH || !control) && eggOn[i] && i == 27 && partyOn == false) &&
                 (millis() - partyTimer >= partyInterval))
        {
          deferredAudMidiNote[layerCount - 1] = (layerCount - 1 + audStart + layerCount);
          MIDI.sendNoteOn(deferredAudMidiNote[layerCount - 1], 120, audChan);
          MIDI.sendNoteOff(deferredAudMidiNote[layerCount - 1], 0, audChan);
          deferredAudMidiNote[layerCount - 1] = 0;
          partyTimer = millis();
          layerTimer[layerCount - 1] = millis();
        }
      }
    }
  }
  // AUDIO layer count trigger

  if (counter != lastCounter || randomVar == true) //add 5 minute step down delay for layer 5 party mode trigger
  {
    trend = counter - lastCounter;

    for (int i = 0; i < layerCount; i++)
    {
      if (((trend > 0 && counter == layer[i]) || (randomVar == true && counter >= layer[i])) && i < layerCount - 1)
      {
        // item is increasing and equal to threshold
        MIDI.sendNoteOn( audStart + randomOffset[i], 120, audChan);
        MIDI.sendNoteOff( audStart + randomOffset[i], 0, audChan);
      }
      else if ((trend > 0 || randomVar == true) && counter == layer[i] && i == layerCount - 1 )
      {
        partyOn = true;
        MIDI.sendNoteOn(i +  audStart, 120, audChan);
        MIDI.sendNoteOff(i + audStart, 0, audChan);
      }
      else if (trend < 0 && counter == (layer[i] - 1) && i != layerCount - 1)
      {
        // item is decreasing and dropping threshold level
        deferredAudMidiNote[i] = (randomOffset[i] + audStart + layerCount);
        layerTimer[i] = millis();
      }
      else if (millis() - partyTimer >= partyInterval && counter != layer[layerCount - 1]) {

        partyOn = false;
        deferredAudMidiNote[layerCount - 1] = (layerCount - 1 + audStart + layerCount);

        MIDI.sendNoteOn(deferredAudMidiNote[layerCount - 1], 120, audChan);
        MIDI.sendNoteOff(deferredAudMidiNote[layerCount - 1], 0, audChan);
        deferredAudMidiNote[layerCount - 1] = 0;
        partyTimer = millis();
        layerTimer[layerCount - 1] = millis();
      }
    }
    if (randomVar == true) {
      randomVar = false;
    }
  }

  for (int i = 0; i < layerCount - 2; i++)
  {
    if (millis() - layerTimer[i] >= counterInterval && deferredAudMidiNote[i] != 0)
    {
      MIDI.sendNoteOn(deferredAudMidiNote[i], 120, audChan);
      MIDI.sendNoteOff(deferredAudMidiNote[i], 0, audChan);
      deferredAudMidiNote[i] = 0;
    }
  }

  if (millis() - lastRandomize >= randomizeInterval)
  {
    randomSeed(analogRead(0));
    lastRandomize = millis();
    for (int i = 0; i < layerCount; i++)
    {
      MIDI.sendNoteOn((i + audStart + layerCount), 120, audChan);
      MIDI.sendNoteOff((i + audStart + layerCount), 0, audChan);
    }
    randomVar = true;

    //Randomize
    for (int a = 0; a < layerCount - 1; a++)
    {
      int r = random(a, layerCount - 1);
      int temp = randomOffset[a];
      randomOffset[a] = randomOffset[r];
      randomOffset[r] = temp;
    }

    //Send new audio on messages
  }
  lastCounter = counter;
}
