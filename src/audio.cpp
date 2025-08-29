#include "audio.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

AudioSystem::AudioSystem() : m_device(nullptr), m_context(nullptr) {}

AudioSystem::~AudioSystem() {
    shutdown();
}

bool AudioSystem::initialize() {
    m_device = alcOpenDevice(nullptr);
    if (!m_device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return false;
    }
    
    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context) {
        std::cerr << "Failed to create OpenAL context" << std::endl;
        alcCloseDevice(m_device);
        m_device = nullptr;
        return false;
    }
    
    if (!alcMakeContextCurrent(m_context)) {
        std::cerr << "Failed to make OpenAL context current" << std::endl;
        alcDestroyContext(m_context);
        alcCloseDevice(m_device);
        m_context = nullptr;
        m_device = nullptr;
        return false;
    }
    
    return true;
}

void AudioSystem::shutdown() {
    for (ALuint source : m_sources) {
        alDeleteSources(1, &source);
    }
    m_sources.clear();
    
    for (ALuint buffer : m_buffers) {
        alDeleteBuffers(1, &buffer);
    }
    m_buffers.clear();
    
    if (m_context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_context);
        m_context = nullptr;
    }
    
    if (m_device) {
        alcCloseDevice(m_device);
        m_device = nullptr;
    }
}

ALuint AudioSystem::loadSound(const std::string& filename) {
    // Simple WAV file loader (for demonstration)
    // In a real implementation, you'd use a proper audio library
    // std::ifstream file(filename, std::ios::binary);
    // if (!file) {
    //     std::cerr << "Failed to open audio file: " << filename << std::endl;
    //     return 0;
    // }
    
    // For now, create a simple procedural sound
    // In practice, you'd load actual WAV files from assets/
    const int sampleRate = 44100;
    const float duration = 1.0f;
    const int numSamples = static_cast<int>(sampleRate * duration);
    
    std::vector<int16_t> samples(numSamples);
    
    ALuint buffer;
    alGenBuffers(1, &buffer);
    
    if (filename.find("throttle") != std::string::npos) {
        // Engine revving sound
        for (int i = 0; i < numSamples; ++i) {
            float t = static_cast<float>(i) / sampleRate;
            float freq = 100.0f + 800.0f * t;
            samples[i] = static_cast<int16_t>(32767.0f * sin(2.0f * 3.14159f * freq * t));
        }
    } else if (filename.find("brake") != std::string::npos) {
        // Brake squeal sound
        for (int i = 0; i < numSamples; ++i) {
            float t = static_cast<float>(i) / sampleRate;
            float freq = 800.0f - 600.0f * t;
            samples[i] = static_cast<int16_t>(32767.0f * sin(2.0f * 3.14159f * freq * t) * (1.0f - t));
        }
    } else {
        // Default sine wave
        for (int i = 0; i < numSamples; ++i) {
            float t = static_cast<float>(i) / sampleRate;
            samples[i] = static_cast<int16_t>(32767.0f * sin(2.0f * 3.14159f * 440.0f * t));
        }
    }
    
    alBufferData(buffer, AL_FORMAT_MONO16, samples.data(), 
                 samples.size() * sizeof(int16_t), sampleRate);
    
    m_buffers.push_back(buffer);
    return buffer;
}

void AudioSystem::playSound(ALuint source, ALuint buffer, float volume, bool loop) {
    alSourcei(source, AL_BUFFER, buffer);
    alSourcef(source, AL_GAIN, volume);
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcePlay(source);
    
    if (checkALError("playSound")) {
        std::cerr << "Failed to play sound" << std::endl;
    }
}

void AudioSystem::stopSound(ALuint source) {
    alSourceStop(source);
    checkALError("stopSound");
}

void AudioSystem::setVolume(ALuint source, float volume) {
    alSourcef(source, AL_GAIN, volume);
    checkALError("setVolume");
}

void AudioSystem::setPitch(ALuint source, float pitch) {
    alSourcef(source, AL_PITCH, pitch);
    checkALError("setPitch");
}

ALuint AudioSystem::createSource() {
    ALuint source;
    alGenSources(1, &source);
    if (checkALError("createSource")) {
        return 0;
    }
    m_sources.push_back(source);
    return source;
}

void AudioSystem::deleteSource(ALuint source) {
    alSourceStop(source);
    alDeleteSources(1, &source);
    auto it = std::find(m_sources.begin(), m_sources.end(), source);
    if (it != m_sources.end()) {
        m_sources.erase(it);
    }
}

bool AudioSystem::isPlaying(ALuint source) const {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

bool AudioSystem::checkALError(const char* operation) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error during " << operation << ": " << error << std::endl;
        return true;
    }
    return false;
}

CarAudio::CarAudio() : m_initialized(false) {}

CarAudio::~CarAudio() {
    shutdown();
}

bool CarAudio::initialize() {
    if (!m_audioSystem.initialize()) {
        return false;
    }
    
    m_throttleSource = m_audioSystem.createSource();
    m_brakeSource = m_audioSystem.createSource();
    m_engineIdleSource = m_audioSystem.createSource();
    m_engineRevSource = m_audioSystem.createSource();
    
    m_throttleBuffer = m_audioSystem.loadSound("throttle");
    m_brakeBuffer = m_audioSystem.loadSound("brake");
    m_engineIdleBuffer = m_audioSystem.loadSound("engine_idle");
    m_engineRevBuffer = m_audioSystem.loadSound("engine_rev");
    
    m_initialized = true;
    return true;
}

void CarAudio::update(float throttle, float brake, float rpm) {
    if (!m_initialized) return;
    
    // Update engine sound based on RPM
    float engineVolume = 0.3f + 0.7f * (rpm / 8000.0f);
    float enginePitch = 0.8f + 1.2f * (rpm / 8000.0f);
    
    m_audioSystem.setVolume(m_engineRevSource, engineVolume);
    m_audioSystem.setPitch(m_engineRevSource, enginePitch);
    
    if (!m_audioSystem.isPlaying(m_engineRevSource)) {
        m_audioSystem.playSound(m_engineRevSource, m_engineRevBuffer, engineVolume, true);
    }
    
    // Handle throttle sound
    if (throttle > 0.1f) {
        playThrottle(throttle);
    } else {
        m_audioSystem.stopSound(m_throttleSource);
    }
    
    // Handle brake sound
    if (brake > 0.1f) {
        playBrake(brake);
    } else {
        m_audioSystem.stopSound(m_brakeSource);
    }
}

void CarAudio::playThrottle(float intensity) {
    if (!m_initialized) return;
    
    float volume = intensity * 0.8f;
    float pitch = 0.8f + intensity * 0.4f;
    
    m_audioSystem.setVolume(m_throttleSource, volume);
    m_audioSystem.setPitch(m_throttleSource, pitch);
    
    if (!m_audioSystem.isPlaying(m_throttleSource)) {
        m_audioSystem.playSound(m_throttleSource, m_throttleBuffer, volume, false);
    }
}

void CarAudio::playBrake(float intensity) {
    if (!m_initialized) return;
    
    float volume = intensity * 1.0f;
    float pitch = 1.0f - intensity * 0.3f;
    
    m_audioSystem.setVolume(m_brakeSource, volume);
    m_audioSystem.setPitch(m_brakeSource, pitch);
    
    if (!m_audioSystem.isPlaying(m_brakeSource)) {
        m_audioSystem.playSound(m_brakeSource, m_brakeBuffer, volume, false);
    }
}

void CarAudio::playEngineIdle() {
    if (!m_initialized) return;
    m_audioSystem.playSound(m_engineIdleSource, m_engineIdleBuffer, 0.2f, true);
}

void CarAudio::playEngineRev(float rpm) {
    if (!m_initialized) return;
    
    float volume = 0.3f + 0.7f * (rpm / 8000.0f);
    float pitch = 0.8f + 1.2f * (rpm / 8000.0f);
    
    m_audioSystem.setVolume(m_engineRevSource, volume);
    m_audioSystem.setPitch(m_engineRevSource, pitch);
    
    if (!m_audioSystem.isPlaying(m_engineRevSource)) {
        m_audioSystem.playSound(m_engineRevSource, m_engineRevBuffer, volume, true);
    }
}

void CarAudio::stopAllSounds() {
    if (!m_initialized) return;
    
    m_audioSystem.stopSound(m_throttleSource);
    m_audioSystem.stopSound(m_brakeSource);
    m_audioSystem.stopSound(m_engineIdleSource);
    m_audioSystem.stopSound(m_engineRevSource);
}

void CarAudio::shutdown() {
    stopAllSounds();
    m_audioSystem.shutdown();
    m_initialized = false;
}