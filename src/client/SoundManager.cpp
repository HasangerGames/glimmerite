#include "gmi/client/SoundManager.h"
#include "gmi/client/gmi.h"

#include "SDL3_mixer/SDL_mixer.h"

namespace gmi {

void SoundManager::init() {
    if (m_initialized) {
        throw GmiException("SoundManager has already been initialized");
    }

    if (!MIX_Init()) {
        throw GmiException(std::string("Unable to initialize SDL_mixer: ") + SDL_GetError());
    }

    const SDL_AudioDeviceID device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (device == 0) {
        throw GmiException(std::string("Unable to initialize audio device: ") + SDL_GetError());
    }

    m_mixer = MIX_CreateMixerDevice(device, nullptr);
    if (m_mixer == nullptr) {
        throw GmiException(std::string("Unable to initialize audio mixer: ") + SDL_GetError());
    }

    m_initialized = true;
}

void SoundManager::load(const std::string& name, const std::string& filePath) {
    m_pendingSounds.emplace(name);
    readFile(
        filePath,
        [this, name](const Buffer& data) {
            SDL_IOStream* stream = SDL_IOFromConstMem(data.data(), data.size());
            if ((m_sounds[name] = MIX_LoadAudio_IO(m_mixer, stream, true, true)) == nullptr) {
                throw GmiException("Error loading sound '" + name + "': " + SDL_GetError());
            }
            m_pendingSounds.erase(name);
        },
        [name, filePath] {
            throw GmiException("Error loading sound '" + name + "': File not found: " + filePath);
        }
    );
}

void SoundManager::play(const std::string& name) {
    if (m_pendingSounds.contains(name)) {
        return;
    }
    if (!m_sounds.contains(name)) {
        throw GmiException("Unknown sound: '" + name + "'");
    }
    if (!MIX_PlayAudio(m_mixer, m_sounds[name])) {
        throw GmiException("Error playing sound '" + name + "': " + SDL_GetError());
    }
}

}
