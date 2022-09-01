#include <NoDelay.h>

#include <Bounce2.h>
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

int bounceInterval = 200;
int counterInterval = 2000;
long randomizeInterval = 18000000; // 12000 debug time

Bounce bouncer[30] = Bounce();

int buttonState[30] = {0};
int prevButtonState[30] = {0};
int entanglements[12] = {0};

int pinStart = 2;
int pinCount = 30;
int dualCount = 24;
int multiCount = 12;
int metaPin = 30;
int eggCount = 6;

int counter = 0;
int prevCounter = 0;

int layer[] = {2, 4, 8, 12, 18, 24}; // 24 is for party mode
int audMidiNotes[5] = {0, 1, 2, 3, 4}

//Timers for actions with delays
long layerTimer[6] = {0};
long lastRandomize = millis();


//Helper Functions
void on(int note, int chan){
    MIDI.sendNoteOn(note, 120, chan);
  };

void off(int note, int chan){
    MIDI.sendNoteOff(note, 0, chan);
  };

void onOff(int note, int chan){
    MIDI.sendNoteOn(note, 120, chan);
    MIDI.sendNoteOff(note, 0, chan);I
  };

bool changed(int i){
    return buttonState[i] != prevButtonState[i];
  }

void handleState()
{
    for (int i = 0; i < 30; i++)
    {
        bouncer[i].update();
        if (bouncer[i].changed())
        {
            prevButtonState[i] = buttonState[i];
            buttonState[i] = bouncer[i].read();
        }
    }
}

void handleCounter()
{
    // update counter and prev counter
    for (int i = 0; i < 24; i++)
        if (changed(i))
        {
            counter++;
        }
        else if (counter != 0)
        {
            counter--;
        }
    }
    // if it changed update prevCounter and send corresponding midi_notes
    if (counter != prevCounter)
    {
        prevCounter = counter;
        //loop through layers
        for (int i = 0; i < 6; i++)
        {
            //if our counter has met a specific threshold
            if (counter == layer[i])
            {
                //and its going up
                if (counter - lastCounter > 0)
                {//send audio on messages
                    MIDI.sendNoteOn(audStart + audMidiNotes[i], 120, audChan);
                    MIDI.sendNoteOff(audStart + audMidiNotes[i], 120, audChan);
                }
                else
                {//if its an off message we are deferring it.
                    layerTimer[i] = millis();
                }
            }
        }
    }
    //check all timers for off messages that have been through their wait
    for (int i = 0; i < 6; i++)
    {
        if (layerTimer[i] != 0 && millis() - layerTimer[i] >= counterInterval)
        {
            onOff((i + audStart + layerCount), audChan);
            //set it back to 0 so this wont constantly fire
            layerTimer[i] = 0;
        }
    }
}

void handleMeta()
{
    if(buttonState[metaPin] == HIGH){
        // Special states in here. 
      }
}


void handleEntanglement()
{
    //get corresponding value [1, 12], [2, 13], [3, 14], [14, 3]...
    // if buttonState changed
    //check that they are both high and add to entanglements array
    for (int i = 0; i < 24; i++)
    {
      if(changed[i])
      {
        if (i < 12)
        {
            if (buttonState[i] == HIGH && buttonState[i + 12] == HIGH)
            {
                entanglements[i] = 1;
                on(i + litStart, litChan);
            }
            else
            {
                entanglements[i] = 0;
                off(i + litStart, litChan);
            }
        }
        if (i >= 12)
        {
            if (buttonState[i] == HIGH && buttonState[i - 12] == HIGH)
            {
                entanglements[i - 12] = 1;
                on(i - 12 + litStart, litChan);
            }
            else
            {
                entanglements[i - 12] = 0;
                off(i - 12 + litStart, litChan);
            }
        }
      }
    }
}

void sendLighting()
{
    for (int i = 0; i <= 24; i++)
    {
        if (changed[i])
        {
            if (buttonState[i] == HIGH)
            {
                MIDI.sendNoteOn(i + litStart, 120, litChan);
            }
            else
            {
                MIDI.sendNoteOff(i + litStart, 0, litChan);
            }
        }
    }
    handleEntanglement();
}

void randomizeAudioNotes()
{
    if (millis() - lastRandomize > randomizeInterval)
    {
        randomSeed(analogRead(0));
        lastRandomize = millis();
//reset all midi notes
        for (int i = 0; i < layerCount; i++)
        {
            onOff((i + audStart + layerCount), audChan);
       }

        for (int i = 0; i < layerCount - 1; i++)
        {
            int r = random(i, layerCount - 1);
            int temp = audMidiNotes[i];
            audMidiNotes[i] = audMidiNotes[r];
            audMidiNotes[r] = temp;
        }
    }
}

void setup()
{
    MIDI.begin(MIDI_CHANNEL_OMNI);
    //initialize global variables so counter starts @ correct value
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
    updateState();
    handleMeta();
    handleCounter();
    handleParty();
    sendLighting();
    sendAudio();
}
