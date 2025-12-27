#include "gmi/client/TweenManager.h"

#include "gmi/client/gmi.h"

namespace gmi {

uint16_t TweenManager::add(const TweenOptions& opts) {
    if (opts.duration <= 0) {
        throw GmiException("Tween duration must be greater than zero");
    }

    auto tween = Tween(opts);
    tween.startTime = nowMs();
    tween.endTime = tween.startTime + tween.opts.duration;
    for (auto& var : tween.opts.values) {
        var.startValue = *var.var;
    }
    uint16_t id = m_nextId;
    m_tweens.emplace(id, tween);
    m_nextId++;
    return id;
}

bool TweenManager::kill(uint16_t id) {
    return static_cast<bool>(m_tweens.erase(id));
}

void TweenManager::update() {
    uint64_t now = nowMs();
    auto iter = m_tweens.begin();
    while (iter != m_tweens.end()) {
        Tween& tween = iter->second;
        TweenOptions& opts = tween.opts;
        const float factor = std::clamp(static_cast<float>(now - tween.startTime) / opts.duration, 0.0f, 1.0f);
        const float eased = opts.ease(factor);

        for (const TweenVar& v : opts.values) {
            if (v.var == nullptr) {
                throw GmiException("Attempted to tween a value that no longer exists");
            }
            *v.var = math::lerp(v.startValue, v.endValue, eased);
        }

        if (opts.onUpdate)
            opts.onUpdate();

        if (now >= tween.endTime) {
            if (opts.yoyo) {
                for (auto& var : opts.values) {
                    std::swap(var.endValue, var.startValue);
                }

                if (!opts.infinite) {
                    opts.yoyo = false;
                }

                tween.startTime = now;
                tween.endTime = tween.startTime + opts.duration;
            } else if (opts.infinite) {
                for (const TweenVar& v : opts.values) {
                    if (v.var == nullptr) {
                        throw GmiException("Attempted to tween a value that no longer exists");
                    }
                    *v.var = v.startValue;
                }

                tween.startTime = now;
                tween.endTime = tween.startTime + opts.duration;
            } else {
                for (const TweenVar& v : opts.values) {
                    if (v.var == nullptr) {
                        throw GmiException("Attempted to tween a value that no longer exists");
                    }
                    *v.var = v.endValue;
                }

                if (opts.onComplete)
                    opts.onComplete();

                iter = m_tweens.erase(iter);
                continue;
            }
        }

        ++iter;
    }
}

}
