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
     * Color color{1.0f, 1.0f, 1.0f, 1.0f};
     *
     * @param r The red component of the color, ranging from 0-255
     * @param g The green component of the color, ranging from 0-255
     * @param b The blue component of the color, ranging from 0-255
     * @return A new Color constructed from the given components
     */
    static Color fromRgb(const float r, const float g, const float b) {
        return {r / 255.0f, g / 255.0f, b / 255.0f, 1.0f};
    }

    /**
     * Creates a Color from red, green, blue, and alpha components, ranging from 0-255.
     *
     * NOTE: If your components range from 0-1, simply create the struct directly:
     *
     * Color color{1.0f, 1.0f, 1.0f, 1.0f};
     *
     * @param r The red component of the color, ranging from 0-255
     * @param g The green component of the color, ranging from 0-255
     * @param b The blue component of the color, ranging from 0-255
     * @param a The alpha component of the color, ranging from 0-255
     * @return A new Color constructed from the given components
     */
    static Color fromRgba(const float r, const float g, const float b, const float a) {
        return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
    }
};

}
