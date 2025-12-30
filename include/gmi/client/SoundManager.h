#pragma once
#include <set>
#include <string>
#include <unordered_map>

#include "SDL3_mixer/SDL_mixer.h"

namespace gmi {

class SoundManager {
public:
    /** Initializes this SoundManager. This method is called internally and should never be called manually. */
    void init();

    /**
     * Loads a sound from disk.
     * @param name The name of the sound
     * @param filePath The path to the sound
     */
    void load(const std::string& name, const std::string& filePath);

    /**
     * Plays a sound.
     * @param name The name of the sound
     */
    void play(const std::string& name);
private:
    bool m_initialized = false;
    MIX_Mixer* m_mixer = nullptr;
    std::unordered_map<std::string, MIX_Audio*> m_sounds;
    std::set<std::string> m_pendingSounds;
};

}
