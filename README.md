# FakeCamera

Henkaku plugin that fakes invalid camera calls in order to avoid some crashes for some titles on PlayStation TV.

Of course, those titles have been blocked by Sony and you must previously unlock them in order to launch them.
Use an application like AntiBlackList (from Rinnegatamante) to do it: http://vitadb.rinnegatamante.it/#/info/11

Once titles are unlocked, they could crash due unexpected SceCamera API answers (because they were never conceived to run on a device without camera). This is where this plugin could intervene to "simulate" expected answers and, therefore, avoid some crashes.
It only works for titles which have been dumped (by Vitamin or MaiDumpTools).


### Installation

For each title which crashes when it should activate the camera, you can add those lines in `ux0:tai/config.txt`:

```
*TITLEID00
ux0:tai/fakecamera.suprx
```

Replace TITLEID00 by your title identifier.


### Compatibility

 * PCSF00007 - WipEout 2048 - The game won't crash on a multiplayer session start! (due to the useless picture feature)
 * PCSF00214 - Tearaway - It won't crash but it will be locked on some asked interactions (like shaking the PS Vita)
