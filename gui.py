
import time
import random
import mido
import sys
import PySimpleGUI as sg
from threading import Thread
from pprint import pprint

bounceInterval = .2
pinStart = 2

# number of pins & corresponding math
pinCount = 30
eEggCount = 6
dualCount = int(pinCount - eEggCount)
multiCount = int(dualCount / 2)


buttonState = ([False] * pinCount)
lastButtonState = ([False] * pinCount)
multiState = ([False] * multiCount)
lastMultiState = ([False] * multiCount)
# how many people are sitting
counter = 0
lastCounter = 0

# midi channel
litChan = 1
multiChan = 2
audChan = 3
eEggChan = 4

# start note
eEgg = 100
litStart = 60
audStart = 40

layer = [2, 4, 8, 12, 18, 24]
layerCount = len(layer)
trend = 0

layerTimer = ([time.time()] * layerCount)
deferredAudMidiNote = ([0] * (layerCount - 1))

lastRandomize = time.time()
randomizeInterval = 1800

randomOffset = [0, 1, 2, 3, 4]
multiVal = ([False] * dualCount)


def setup():
    output_names = mido.get_output_names()
    print('output names -->', output_names)
    # attach bouncers
    global port
    port = mido.open_output(output_names[2])


def loop(threadname):
    print('multicount', multiCount)
    while True:
        try:
            for index in range(pinCount):
                if buttonState[index] != lastButtonState[index]:
                    lastButtonState[index] = buttonState[index]
                    if index < multiCount - 1:
                        if buttonState[index + multiCount - 1] == True and buttonState[index] == True:
                            noteOn(index + litStart, multiChan)
                            global multiVal
                            multiVal[index] = True
                        elif multiVal[index] == True:
                            noteOff(index + litStart, multiChan)
                            multiVal[index] = False
                    if index >= multiCount - 1:
                        if buttonState[(index - (multiCount - 1))] == True and buttonState[index] == True:
                            noteOn(index + litStart -
                                   (multiCount - 1), multiChan)
                            multiVal[index] = True
                        else:
                            if multiVal[index] == True:
                                noteOff(index + litStart -
                                        (multiCount - 1), multiChan)
                                multiVal[index] = False
        except Exception as e:
            print(e)


def noteOn(note, chan):
    print('note_on: ', note, 'chan: ', chan)
    msg = mido.Message('note_on', note=note, velocity=120, channel=chan)
    port.send(msg)


def noteOff(note, chan):
    print('note_off: ', note, 'chan: ', chan)
    msg = mido.Message('note_off', note=note, velocity=0, channel=chan)
    port.send(msg)


def noteOnOff(note, chan):
    print('note: ', note, 'chan: ', chan)
    msg = mido.Message('note_on', note=note, velocity=120, channel=chan)
    port.send(msg)
    msg = mido.Message('note_off', note=note, velocity=0, channel=chan)
    port.send(msg)


def main(threadname):
    layout = [[sg.Button("1"), sg.Text(buttonState[0], key="t1"),
              sg.Button("2"), sg.Text(buttonState[1], key="t2"),
              sg.Button("3"), sg.Text(buttonState[2], key="t3"),
              sg.Button("4"), sg.Text(buttonState[3], key="t4")],
              [sg.Button("5"), sg.Text(buttonState[4], key="t5"),
              sg.Button("6"), sg.Text(buttonState[5], key="t6"),
              sg.Button("7"), sg.Text(buttonState[6], key="t7"),
              sg.Button("8"), sg.Text(buttonState[7], key="t8")],
              [sg.Button("9"), sg.Text(buttonState[8], key="t9"),
              sg.Button("10"), sg.Text(buttonState[9], key="t10"),
              sg.Button("11"), sg.Text(buttonState[10], key="t11"),
              sg.Button("12"), sg.Text(buttonState[11], key="t12")],
              [sg.Button("13"), sg.Text(buttonState[12], key="t13"),
              sg.Button("14"), sg.Text(buttonState[13], key="t14"),
              sg.Button("15"), sg.Text(buttonState[14], key="t15"),
              sg.Button("16"), sg.Text(buttonState[15], key="t16")],
              [sg.Button("17"), sg.Text(buttonState[16], key="t17"),
              sg.Button("18"), sg.Text(buttonState[17], key="t18"),
              sg.Button("19"), sg.Text(buttonState[18], key="t19"),
              sg.Button("20"), sg.Text(buttonState[19], key="t20")],
              [sg.Button("21"), sg.Text(buttonState[20], key="t21"),
              sg.Button("22"), sg.Text(buttonState[21], key="t22"),
              sg.Button("23"), sg.Text(buttonState[22], key="t23"),
              sg.Button("24"), sg.Text(buttonState[23], key="t24")],
              ]
    window = sg.Window("Main Window", layout)
    while True:
        event, values = window.read()
        if event == "1":
            buttonState[0] = not buttonState[0]
            window['t1'].update(buttonState[0])
        if event == "2":
            buttonState[1] = not buttonState[1]
            window['t2'].update(buttonState[1])
        if event == "3":
            buttonState[2] = not buttonState[2]
            window['t3'].update(buttonState[2])
        if event == "4":
            buttonState[3] = not buttonState[3]
            window['t4'].update(buttonState[3])
        if event == "5":
            buttonState[4] = not buttonState[4]
            window['t5'].update(buttonState[4])
        if event == "6":
            buttonState[5] = not buttonState[5]
            window['t6'].update(buttonState[5])
        if event == "7":
            buttonState[6] = not buttonState[6]
            window['t7'].update(buttonState[6])
        if event == "8":
            buttonState[7] = not buttonState[7]
            window['t8'].update(buttonState[7])
        if event == "9":
            buttonState[8] = not buttonState[8]
            window['t9'].update(buttonState[8])
        if event == "10":
            buttonState[9] = not buttonState[9]
            window['t10'].update(buttonState[9])
        if event == "11":
            buttonState[10] = not buttonState[10]
            window['t11'].update(buttonState[10])
        if event == "12":
            buttonState[11] = not buttonState[11]
            window['t12'].update(buttonState[11])
        if event == "13":
            buttonState[12] = not buttonState[12]
            window['t13'].update(buttonState[12])
        if event == "14":
            buttonState[13] = not buttonState[13]
            window['t14'].update(buttonState[13])
        if event == "15":
            buttonState[14] = not buttonState[14]
            window['t15'].update(buttonState[14])
        if event == "16":
            buttonState[15] = not buttonState[15]
            window['t16'].update(buttonState[15])
        if event == "17":
            buttonState[16] = not buttonState[16]
            window['t17'].update(buttonState[16])
        if event == "18":
            buttonState[17] = not buttonState[17]
            window['t18'].update(buttonState[17])
        if event == "19":
            buttonState[18] = not buttonState[18]
            window['t19'].update(buttonState[18])
        if event == "20":
            buttonState[19] = not buttonState[19]
            window['t20'].update(buttonState[19])
        if event == "21":
            buttonState[20] = not buttonState[20]
            window['t21'].update(buttonState[20])
        if event == "22":
            buttonState[21] = not buttonState[21]
            window['t22'].update(buttonState[21])
        if event == "23":
            buttonState[22] = not buttonState[22]
            window['t23'].update(buttonState[22])
        if event == "24":
            buttonState[23] = not buttonState[23]
            window['t24'].update(buttonState[23])

        if event == "Exit" or event == sg.WIN_CLOSED:
            break
    window.close()


if __name__ == "__main__":
    setup()
    t = Thread(target=loop, args=('Loop ',))
    t1 = Thread(target=main, args=('Main ',))
    t.start()
    t1.start()
    t.join()
    t1.join()
