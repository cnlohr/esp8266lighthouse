# esp8266lighthouse

## Internal notes:

```
NOTES for transimpedence2/3
TESTS Performed with no C1, TSV992
Making R4 220k instead of 47k seeems to have little effect, but does make it worse.
Switching R1 for 47k just makes it appear drunk. There's some weird feedback thing going on.
    Like it tuned wrong or something.   Making it 10k quiets the signal a lot.
REMOVING C4 no apparent impact.

R5=10k, C5=10p -> R5=4.7k, C5=18p =>> Improvement.
Adding C1=1pF decreases amplitude a little bit but also cleans up edges.
Removing C6 seems to reduce amplitude slightly.

Far peak:
C1 none C6 none => .1238V noise, .998V Far Light
C1 none C6 1pF    => .1238V noise, .498V Far Light
C1 1pF  C6 none => .1135V noise, .358V Far Light
C1 1pF C6 1pF     => .087V Noise, .202V Far Light

Recommendation: Remove C6, C1.

Out of the following op amps...
TSH82, TSV992, AD8052, TSH22, TSH462,
TSV992 demolished.


Useful photodiodes:
PART / NOISE / FAR / Cost
PD70-01C/TR7:  .159 / .418 / .47
PD438C: .237 / .550 / .39
QSD2030F: .216 / .5363 / .30
BPW34/S-Z: .283 / .6345 /.497
PD15-216/TR8: UNUSABLE
VBPW34FAS: .175 / .815 / .778
VBPW34FASR: .2631 / .644 / .778
BPW34: .237 / .577 / .62
PD15-22C/TR8: Unworkable.
PD15-22B/TR8: Unworkable.
PD70-TR10: .2063 / .4024 / .41
BPW46: .247 / .968 / .61

Winners: BPW46, BPW34/S-Z, BPW34FAS + VBPW34FASR
Major Winners: BPW46/VBPW34FAS
```
