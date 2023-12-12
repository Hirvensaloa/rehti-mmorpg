
#include <soloud.h>
#include <soloud_wav.h>

class RehtiAudio
{
public:
    RehtiAudio();
    ~RehtiAudio();

    void playMusic();

private:
    SoLoud::Soloud audioEngineM;

    SoLoud::Wav mainMusicM;
};