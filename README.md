# SDL Audio Examples

This is a repository of basic examples on how to use
[SDL](https://wiki.libsdl.org/)'s audio functionality.  I plan to create basic
examples for future reference or as a starting point for anyone interested in
working with SDL and audio. I have also over commented the code for extra
explanation where needed.

## Dependencies

Download SDL2 if needed. This code uses functionality introduced in version
2.0.4 so make sure to get the latest version. I use [homebrew](https://brew.sh/)
but any package manager/direct install should work.

```
brew install sdl2 sdl2_mixer
```

## Build and run the example code

The build script uses clang to compile.

```
git clone https://github.com/dan-sf/sdl-audio-examples.git
cd sdl-audio-examples
./build.sh
# Run executables (.out files)
./wav_callback.out
./wav_mix.out
# etc.
```

