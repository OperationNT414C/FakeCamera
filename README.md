# FakeCamera

Henkaku plugin that fakes invalid camera calls in order to avoid some crashes for some titles on PlayStation TV.

Of course, those titles have been blocked by Sony and you must previously unlock them in order to launch them.
Use an application like AntiBlackList (from Rinnegatamante) to do it:
http://vitadb.rinnegatamante.it/#/info/11

Once titles are unlocked, they could crash due unexpected SceCamera API answers (because they were never conceived to run on a device without camera). This is where this plugin could intervene to "simulate" expected answers and, therefore, avoid some crashes.

With "fakecamerabmp.suprx" plugin, a BMP file image can be loaded and used as camera output. Images must be placed in "ux0:data/FakeCamera" directory. An image is selected in this directory with the following priority:
 * "ux0:data/FakeCamera/TITLEID00_Front.bmp" or "ux0:data/FakeCamera/TITLEID00_Back.bmp" (depends on front or back camera use)
 * "ux0:data/FakeCamera/TITLEID00.bmp"
 * "ux0:data/FakeCamera/ALL_Front.bmp" or "ux0:data/FakeCamera/ALL_Back.bmp" (depends on front or back camera use)
 * "ux0:data/FakeCamera/ALL.bmp"


### Dependencies

The plugin "fakecamera.suprx" doesn't have any dependency.

The plugin "fakecamerabmp.suprx" depends on 2 kernel plugins:
 * **kuio.skprx** (https://github.com/Rinnegatamante/kuio) for "ux0:data" access even with signed titles
 * **dsmotion.skprx** (https://github.com/OperationNT414C/DSMotion) for image scrolling with motion controls
Those dependencies must be loaded otherwise "fakecamerabmp.suprx" won't load.


### Installation

For each title which crashes when it should activate the camera, you can add those lines in `ux0:tai/config.txt`:

```
*TITLEID00
ux0:tai/fakecamera.suprx
```

OR (even if the title doesn't crash, it will allow you to set up a BMP image as camera output)

```
*KERNEL
ux0:tai/dsmotion.skprx
ux0:tai/kuio.skprx

*TITLEID00
ux0:tai/fakecamerabmp.suprx
```

Replace **TITLEID00** by your title identifier or by **ALL** to affect all titles.

DO NOT use both "fakecamera.suprx" and "fakecamerabmp.suprx" on the same configuration!


### Compatibility

 * PCSF00007 - WipEout 2048 - The game won't crash on a multiplayer session start! (due to the useless picture feature)
 * PCSF00214 - Tearaway - It won't crash but it will be locked on some asked interactions (like shaking the PS Vita)


### BMP load compatibility

The plugin version which allow a BMP image as camera output works on very limited applications list:
 * PCM300001 - Pro Camera Vita - Works fine
 * NPXS10007 - Welcome Park - Works fine in "Hello Face" mini-game but doesn't work in "Snap + Slide" mini-game


### Credits

 * **Rinnegatamante** for his "kuio" kernel plugin
 * **xerpi** for his "libvita2d" source code which inspired me for BMP format read
