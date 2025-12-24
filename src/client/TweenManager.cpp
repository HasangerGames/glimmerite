#include "gmi/client/TweenManager.h"

#include "gmi/client/gmi.h"

namespace gmi {

void TweenManager::add(const TweenOptions& opts) {
    if (opts.duration <= 0) {
        throw GmiException("Tween duration must be greater than zero");
    }
    auto tween = Tween(opts);
    tween.startTime = nowMs();
    tween.endTime = tween.startTime + tween.opts.duration;
    for (auto& var : tween.opts.values) {
        var.startValue = *var.var;
    }
    m_tweens.push_back(tween);
}

void TweenManager::update() {
    uint64_t now = nowMs();
    auto tween = m_tweens.begin();
    while (tween != m_tweens.end()) {
        TweenOptions& opts = tween->opts;
        const float factor = std::clamp(static_cast<float>(now - tween->startTime) / opts.duration, 0.0f, 1.0f);
        const float eased = opts.ease(factor);

        for (const TweenVar& v : opts.values) {
            if (v.var == nullptr) {
                throw GmiException("Attempted to tween a value that no longer exists");
            }
            *v.var = math::lerp(v.startValue, v.endValue, eased);
        }

        if (opts.onUpdate)
            opts.onUpdate();

        if (now >= tween->endTime) {
            if (opts.yoyo) {
                for (auto& var : opts.values) {
                    std::swap(var.endValue, var.startValue);
                }

                if (!opts.infinite) {
                    opts.yoyo = false;
                }

                tween->startTime = now;
                tween->endTime = tween->startTime + opts.duration;
            } else if (opts.infinite) {
                for (const TweenVar& v : opts.values) {
                    if (v.var == nullptr) {
                        throw GmiException("Attempted to tween a value that no longer exists");
                    }
                    *v.var = v.startValue;
                }

                tween->startTime = now;
                tween->endTime = tween->startTime + opts.duration;
            } else {
                for (const TweenVar& v : opts.values) {
                    if (v.var == nullptr) {
                        throw GmiException("Attempted to tween a value that no longer exists");
                    }
                    *v.var = v.endValue;
                }

                if (opts.onComplete)
                    opts.onComplete();

                m_tweens.erase(tween);
                continue;
            }
        }

        ++tween;
    }
}

}
