#include "RehtiAudio.hpp"

RehtiAudio::RehtiAudio() : audioEngineM{}, mainMusicM{}
{
    audioEngineM.init();
    mainMusicM.load("./assets/audio/Al_Kharid_Quantum_Kebab_Mix.wav");
}

RehtiAudio::~RehtiAudio()
{
    audioEngineM.deinit();
}

void RehtiAudio::playMusic()
{
    int mainMusicHandle = audioEngineM.play(mainMusicM);
    audioEngineM.setLooping(mainMusicHandle, true);
}