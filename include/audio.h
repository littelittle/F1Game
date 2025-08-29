#ifndef AUDIO_H
#define AUDIO_H

#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <string>

class AudioSystem {
public:
    AudioSystem();
    ~AudioSystem();
    
    bool initialize();
    void shutdown();
    
    ALuint loadSound(const std::string& filename);
    void playSound(ALuint source, ALuint buffer, float volume = 1.0f, bool loop = false);
    void stopSound(ALuint source);
    void setVolume(ALuint source, float volume);
    void setPitch(ALuint source, float pitch);
    
    ALuint createSource();
    void deleteSource(ALuint source);
    
    bool isPlaying(ALuint source) const;
    
private:
    ALCdevice* m_device;
    ALCcontext* m_context;
    std::vector<ALuint> m_sources;
    std::vector<ALuint> m_buffers;
    
    bool checkALError(const char* operation);
};

class CarAudio {
public:
    CarAudio();
    ~CarAudio();
    
    bool initialize();
    void update(float throttle, float brake, float rpm);
    void shutdown();
    
    void playThrottle(float intensity);
    void playBrake(float intensity);
    void playEngineIdle();
    void playEngineRev(float rpm);
    
    void stopAllSounds();
    
private:
    AudioSystem m_audioSystem;
    ALuint m_throttleSource;
    ALuint m_brakeSource;
    ALuint m_engineIdleSource;
    ALuint m_engineRevSource;
    
    ALuint m_throttleBuffer;
    ALuint m_brakeBuffer;
    ALuint m_engineIdleBuffer;
    ALuint m_engineRevBuffer;
    
    bool m_initialized;
};

#endif // AUDIO_H