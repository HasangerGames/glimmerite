#pragma once

namespace gmi {

/**
 * Represents an RGBA color.
 * Each component (red, green, blue, alpha) is stored as a normalized float ranging from 0-1.
 */
struct Color {
    /** The red component of the color, ranging from 0-1. */
    float r = 1.0f;
    /** The green component of the color, ranging from 0-1. */
    float g = 1.0f;
    /** The blue component of the color, ranging from 0-1. */
    float b = 1.0f;
    /** The alpha component of the color, ranging from 0-1. */
    float a = 1.0f;

    /**
     * Creates a Color from red, green, and blue components, ranging from 0-255.
     *
     * NOTE: If your components range from 0-1, simply create the struct directly:
     *
     * Color color = {1.0f, 1.0f, 1.0f, 1.0f};
     *
     * @param r The red component of the color, ranging from 0-255
     * @param g The green component of the color, ranging from 0-255
     * @param b The blue component of the color, ranging from 0-255
     * @return A new Color constructed from the given components
     */
    static Color fromRgb(float r, float g, float b) {
        return {r / 255.0f, g / 255.0f, b / 255.0f, 1.0f};
    }

    /**
     * Creates a Color from red, green, blue, and alpha components, ranging from 0-255.
     *
     * NOTE: If your components range from 0-1, simply create the struct directly:
     *
     * Color color = {1.0f, 1.0f, 1.0f, 1.0f};
     *
     * @param r The red component of the color, ranging from 0-255
     * @param g The green component of the color, ranging from 0-255
     * @param b The blue component of the color, ranging from 0-255
     * @param a The alpha component of the color, ranging from 0-255
     * @return A new Color constructed from the given components
     */
    static Color fromRgba(float r, float g, float b, float a) {
        return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
    }

    /**
     * Creates a Color given a hue, saturation, lightness, and alpha.
     * @param h The hue component of the color, ranging from 0-1
     * @param s The saturation component of the color, ranging from 0-1
     * @param l The lightness component of the color, ranging from 0-1
     * @param a The alpha component of the color, ranging from 0-1
     * @return A new Color constructed from the given components
     */
    static Color fromHsla(float h, float s, float l, float a) {
        // Adapted from https://gist.github.com/ciembor/1494530

        // Converts a HUE to r, g or b. Returns a float in the set [0, 1].
        constexpr auto hue2rgb = [](float p, float q, float t) -> float {
            if (t < 0)
                t += 1;
            if (t > 1)
                t -= 1;
            if (t < 1./6)
                return p + (q - p) * 6 * t;
            if (t < 1./2)
                return q;
            if (t < 2./3)
                return p + (q - p) * (2./3 - t) * 6;

            return p;
        };

        if (s == 0) {
            return {l, l, l}; // achromatic
        }

        float q{l < 0.5 ? l * (1 + s) : l + s - l * s};
        float p{2 * l - q};
        return {
            .r = hue2rgb(p, q, h + 1./3),
            .g = hue2rgb(p, q, h),
            .b = hue2rgb(p, q, h - 1./3),
            .a = a
        };
    }

    /**
     * Creates a Color given a hue, saturation, and lightness.
     * @param h The hue component of the color, ranging from 0-1
     * @param s The saturation component of the color, ranging from 0-1
     * @param l The lightness component of the color, ranging from 0-1
     * @return A new Color constructed from the given components
     */
    static Color fromHsl(float h, float s, float l) {
        return fromHsla(h, s, l, 1.0f);
    }
};

inline std::ostream& operator<<(std::ostream& stream, const Color& c) {
    stream << "Color{r=" << c.r << ", g=" << c.g << ", b=" << c.b << ", a=" << c.a << "}";
    return stream;
}

inline Color operator*(const Color& a, const Color& b) {
    return {a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a};
}

inline uint32_t colorToNumber(const Color& color) {
    const uint8_t r = color.r * 255,
                  g = color.g * 255,
                  b = color.b * 255,
                  a = color.a * 255;
    return (r << 24) | (g << 16) | (b << 8) | a;
}

}
