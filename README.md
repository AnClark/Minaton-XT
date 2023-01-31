# Minaton-XT

**Minaton** - A monophonic, subtractive, beefy analogue style bass and lead synthesizer. Originally written by Nick Bailey (ThunderOx Software) ([@thunderox](https://github.com/thunderox/Minaton))

This project, **Minaton-XT**, ports Minaton to [DPF, DISTRHO Plugin Framework](https://distrho.github.io/DPF/).

With DPF powered, Minaton is alive again. It is not limited to the only LV2 format any more, instead it can run in multiple formats. What's more, it's now cross-platform as well, and will benefit musicians in Linux, Windows and macOS.

## Description from ThunderOx

This is my first music software synthesizer project, developed over the last two weeks it's now in a usable state but there are still things I wan't to do, In using floating point values for all parameters I tried to get away from the steppy digital sounds you often hear on software synthesizers, I've used real analogue synthesizers since the late 1980s so know how they should sound, deep, warm, fluid and rich.

The synth has three oscillators, two LFOs, One low pass resonant filter, oscillator sync, two ADSR envelopes which can be redirected to oscillator pitch. The mod wheel controls filter frequency. It uses jackmidi rather than alsa for good solid timing, future developments planned are a lv2 / ladspa plug in version, pitch wheel control to be added.

Example sounds can be found on my blog, any suggestions and additions would be appreciated.

## Features of This Port

- **Further development:** A bunch of work to make Minaton work in modern hosts, including essential bug fixes. For instance:
  - Output sanitizer (fix occational out-of-range frames)
  - LFO
  - Volume control
  - Keyboard play in legato
  - Read wave samples from memory
  - ... and so on.
- **Multiple plug-in format support:** VST2, VST3, LV2, CLAP
- **Cross-platform:** Linux, Windows, macOS
- **Compatibility:** Resolved compatibility issue from old LV2 specifications (mainly in REAPER)
- **Embedded resources:** Everything is in a single plug-in file.

## Build Instructions

### 1. Clone source tree

Source tree has three submodules. One is DPF, and the other two are dependency libraries: libsamplerate, libsndfine.

```bash
git clone https://github.com/AnClark/Minaton-XT.git minaton --recursive

# If you forget --recursive, run this
cd minaton
git submodule update --init --recursive
```

### 2. Build (using bundled dependencies)

Building with bundled libsamplerate and libsndfile is highly recommended. This can largely reduce external dependencies, especially on Windows.

For best performance, `Release` build is recommended. Optionally you can also set build type to `Debug`.

```bash
cd minaton
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Built plug-ins reside in `build/bin`.

### 3. Build (using system-provided dependencies)

On Linux, you can directly use those libraries installed by your package manager.

```bash
cd minaton
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DUSE_SYSTEM_LIBSAMPLERATE=ON -DUSE_SYSTEM_LIBSNDFILE=ON
cmake --build build
```

### Notice

The `makefile` in `src/` subdirectory is used to build the old LV2 plugin, and not used by Minaton-XT.

## License

Minaton is licensed under GNU General Public License, Version 3.
