#pragma once

#include <algorithm>
#include <cstdint>
#include <format>

namespace gmi {

/** Represents a color with red, green, blue, and alpha components. Each component is stored as a uint8_t. */
struct Color {
    /** The red component of the color. */
    uint8_t r = 255;
    /** The green component of the color. */
    uint8_t g = 255;
    /** The blue component of the color. */
    uint8_t b = 255;
    /** The alpha component of the color. */
    uint8_t a = 255;

    /**
     * Creates a Color given red, green, blue, and alpha as uint8_t ranging from 0-255.
     * @param r The red component of the color
     * @param g The green component of the color
     * @param b The blue component of the color
     * @param a The alpha component of the color
     * @return A new Color created from the given components
     */
    static Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    /**
     * Creates a Color given red, green, and blue as uint8_t ranging from 0-255.
     * @param r The red component of the color
     * @param g The green component of the color
     * @param b The blue component of the color
     * @return A new Color created from the given components
     */
    static Color rgb(uint8_t r, uint8_t g, uint8_t b);

    /**
     * Creates a Color from an RGBA hex value.
     * @param hex The hex value
     * @return A new Color created from the given hex value
     */
    static Color rgbaHex(uint32_t hex);

    /**
     * Creates a Color given red, green, blue, and alpha as normalized floats ranging from 0-1.
     * @param r The red component of the color
     * @param g The green component of the color
     * @param b The blue component of the color
     * @param a The alpha component of the color
     * @return A new Color created from the given components
     */
    static Color rgbaFloat(float r, float g, float b, float a);

    /**
     * Creates a Color given hue, saturation, lightness, and alpha as normalized floats ranging from 0-1.
     * @param h The hue component of the color
     * @param s The saturation component of the color
     * @param l The lightness component of the color
     * @param a The alpha component of the color
     * @return A new Color constructed from the given components
     */
    static Color hsla(float h, float s, float l, float a);

    /**
     * Creates a Color given hue, saturation, and lightness as normalized floats ranging from 0-1.
     * @param h The hue component of the color
     * @param s The saturation component of the color
     * @param l The lightness component of the color
     * @return A new Color constructed from the given components
     */
    static Color hsl(float h, float s, float l);

    /** @return A human-readable representation of this Color */
    [[nodiscard]] std::string toString() const;

    /** @return The RGBA hex value of this Color */
    [[nodiscard]] uint32_t rgbaHex() const;

    static const Color White, Black, Red, Green, Blue, Yellow, Cyan, Magenta;
};

inline const Color Color::White   = rgb(255, 255, 255);
inline const Color Color::Black   = rgb(0,   0,   0  );
inline const Color Color::Red     = rgb(255, 0,   0  );
inline const Color Color::Green   = rgb(0,   255, 0  );
inline const Color Color::Blue    = rgb(0,   0,   255);
inline const Color Color::Yellow  = rgb(255, 255, 0  );
inline const Color Color::Cyan    = rgb(0,   255, 255);
inline const Color Color::Magenta = rgb(255, 0,   255);

inline Color Color::rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return {r, g, b, a};
}

inline Color Color::rgb(uint8_t r, uint8_t g, uint8_t b) {
    return {r, g, b, 255};
}

inline Color Color::rgbaFloat(float r, float g, float b, float a) {
    return {
        static_cast<uint8_t>(std::clamp(r, 0.0f, 1.0f) * 255),
        static_cast<uint8_t>(std::clamp(g, 0.0f, 1.0f) * 255),
        static_cast<uint8_t>(std::clamp(b, 0.0f, 1.0f) * 255),
        static_cast<uint8_t>(std::clamp(a, 0.0f, 1.0f) * 255),
    };
}

inline Color Color::rgbaHex(uint32_t hex) {
    return {
        static_cast<uint8_t>(hex >> 24 & 0xff),
        static_cast<uint8_t>(hex >> 16 & 0xff),
        static_cast<uint8_t>(hex >> 8  & 0xff),
        static_cast<uint8_t>(hex       & 0xff),
    };
}

inline Color Color::hsla(float h, float s, float l, float a) {
    // Adapted from https://gist.github.com/ciembor/1494530

    // Converts a HUE to r, g or b. Returns a float in the set [0, 1].
    constexpr auto hue2rgb = [](float p, float q, float t) -> float {
        if (t < 0.0f)
            t += 1.0f;
        if (t > 1.0f)
            t -= 1.0f;
        if (t < 1.0f / 6.0f)
            return p + (q - p) * 6.0f * t;
        if (t < 1.0f / 2.0f)
            return q;
        if (t < 2.0f / 3.0f)
            return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;

        return p;
    };

    if (s == 0.0f) {
        return rgbaFloat(l, l, l, 1.0f); // achromatic
    }

    float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
    float p = 2.0f * l - q;
    return rgbaFloat(
        hue2rgb(p, q, h + 1.0f / 3.0f),
        hue2rgb(p, q, h),
        hue2rgb(p, q, h - 1.0f / 3.0f),
        a
    );
}

inline Color Color::hsl(float h, float s, float l) {
    return hsla(h, s, l, 1.0f);
}

inline std::string Color::toString() const {
    return std::format(
        "Color{{r={}, g={}, b={}, a={}}}",
        r, g, b, a
    );
}

inline uint32_t Color::rgbaHex() const {
    return r << 24 | g << 16 | b << 8 | a;
}

constexpr std::ostream& operator<<(std::ostream& stream, Color c) {
    stream << c.toString();
    return stream;
}

constexpr Color operator*(Color a, Color b) {
    return {
        static_cast<uint8_t>(a.r * b.r / 255),
        static_cast<uint8_t>(a.g * b.g / 255),
        static_cast<uint8_t>(a.b * b.b / 255),
        static_cast<uint8_t>(a.a * b.a / 255),
    };
}

constexpr Color operator*(Color c, float s) {
    s = std::clamp(s, 0.0f, 1.0f);
    return {
        static_cast<uint8_t>(static_cast<float>(c.r) * s),
        static_cast<uint8_t>(static_cast<float>(c.g) * s),
        static_cast<uint8_t>(static_cast<float>(c.b) * s),
        static_cast<uint8_t>(static_cast<float>(c.a) * s),
    };
}

}
