I'm too lazy to do things right, so I just forked https://sourceforge.net/projects/joy2key/ and hacked it so it works for me.

Config for 4 key mode on a yuan another IIDX controller:
```
./joy2keyng -buttons 6 d 4 f 3 space 2 j 0 k 7 Up 8 Return 9 Down 5 Left 1 Right
```

# HORI Taiko no Tatsujin controller for Nintendo Switch:

|Keycode| Button|
|-------|-------|
|0      |Y|
|1      |B|
|2      |A|
|3      |X|
|4      |L button|
|5      |R button|
|6      |Left Katsu|
|7      |Right Katu|
|8      |- (minus)|
|9      |+ (plus)|
|10     |Left Don|
|11     |Right Don|
|12     |Home|
|13     |Square (screenshot)|

```
# To bind:
# d = left katsu
# f = left don
# j = right don
# k = right katsu

./joy2keyng -buttons 6 d 10 f 11 j 7 k

```
