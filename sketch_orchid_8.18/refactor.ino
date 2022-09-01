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
int eggCount = 6;

int counter = 0;
int prevCounter = 0;
int layer[] = {2, 4, 8, 12, 18, 24}; // 24 is for party mode
int audMidiNotes[5] = {0, 1, 2, 3, 4}

long layerTimer[6] = {0};
long lastRandomize = millis();

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
    {//if the buttons pressed
        if (buttonState[i] == HIGH)
        {
            prevCounter = counter;
            counter++;
        }
        else
        {
            prevCounter = counter;
            if (counter != 0)
            {
                counter--;
            }
        }
    }
    // if it changed send corresponding midi notes
    if (counter != prevCounter)
    {
        //loop through layers
        for (int i = 0; i < 6; i++)
        {
            //if our counter is on a specific threshold
            if (counter == layer[i])
            {
                //and its going up
                if (counter - lastCounter > 0)
                {//send audio on messages
                    MIDI.sendNoteOn(audStart + audMidiNotes[i], 120, audChan);
                    MIDI.sendNoteOff(audStart + audMidiNotes[i], 120, audChan);
                }
                else
                {//if its an on message we are deferring it.
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
            MIDI.sendNoteOn((i + audStart + layerCount), 120, audChan);
            MIDI.sendNoteOff((i + audStart + layerCount), 0, audChan);
            //set it back to 0 so this wont constantly fire
            layerTimer[i] = 0;
        }
    }
}

void handleEntanglement()
{
    //get 
    for (int i = 0; i <= 24; i++)
    {
        if (i < 12)
        {
            if (buttonState[i] == HIGH && buttonState[i + 12] == HIGH)
            {
                entanglements[i] = 1;
            }
            else
            {
                entanglements[i] = 0;
            }
        }
        if (i >= 12)
        {
            if (buttonState[i] == HIGH && buttonState[i - 12] == HIGH)
            {
                entanglements[i - 12] = 1;
            }
            else
            {
                entanglements[i - 12] = 0;
            }
        }
    }
}

void handleLighting()
{
    for (int i = 0; i <= 24; i++)
    {
        if (buttonState[i] != prevButtonState[i])
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

void randomize()
{
    if (millis() - lastRandomize > randomizeInterval)
    {
        randomSeed(analogRead(0));
        lastRandomize = millis();

        // for (int i = 0; i < layerCount; i++)
        // {
        //     MIDI.sendNoteOn((i + audStart + layerCount), 120, audChan);
        //     MIDI.sendNoteOff((i + audStart + layerCount), 0, audChan);
        // }
        // randomVar = true;

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
    handleState();
    handleLighting();
    handleCounter();
}