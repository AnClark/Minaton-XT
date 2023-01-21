# Minaton-DPF

**Minaton** - A monophonic, subtractive, beefy analogue style bass and lead synthesizer. Originally written by Nick Bailey (ThunderOx Software) ([@thunderox](https://github.com/thunderox/Minaton))

This project, **Minaton-DPF**, ports Minaton to [DPF, DISTRHO Plugin Framework](https://distrho.github.io/DPF/).

With DPF powered, Minaton is alive again. It is not limited to the only LV2 format any more, instead it can run in multiple formats. What's more, it's now cross-platform as well. It will benefit musicians in Linux, Windows and macOS.

## Description from ThunderOx

This is my first music software synthesizer project, developed over the last two weeks it's now in a usable state but there are still things I wan't to do, In using floating point values for all parameters I tried to get away from the steppy digital sounds you often hear on software synthesizers, I've used real analogue synthesizers since the late 1980s so know how they should sound, deep, warm, fluid and rich.

The synth has three oscillators, two LFOs, One low pass resonant filter, oscillator sync, two ADSR envelopes which can be redirected to oscillator pitch. The mod wheel controls filter frequency. It uses jackmidi rather than alsa for good solid timing, future developments planned are a lv2 / ladspa plug in version, pitch wheel control to be added.

Example sounds can be found on my blog, any suggestions and additions would be appreciated.

## Features of This Port

- **Multiple plug-in format support:** VST2, VST3, LV2, CLAP
- **Cross-platform:** Linux, Windows, macOS
- **Compatibility:** Resolved compatibility issue from old LV2 specifications (mainly in REAPER)

## Build Instructions

```bash
git clone https://github.com/AnClark/Minaton-DPF.git --recursive
cd Minaton-DPF
cmake -S . -B build
cmake --build build
```

Built plug-ins reside in `build/bin`.

## License

Minaton is licensed under GNU General Public License, Version 3.