#include "backends/sdl/SdlBackend.h"

#include <iostream>
#include <thread>

#include "SdlTexture.h"
#include "gmi/Application.h"
#include "gmi/gmi.h"
#include "../../../include/gmi/Color.h"
#include "internal/utils.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"

namespace gmi {

SdlBackend::SdlBackend(const Application& parentApp, RendererType rendererType) : Backend(parentApp, rendererType) {
    // Auto-detect best renderer / fall back if provided renderer is not supported
    std::set supportedRenderers{getSupportedRenderers(BackendType::Sdl)};
    if (rendererType == RendererType::Auto || !supportedRenderers.contains(rendererType)) {
        for (int i = 0, len = std::size(PREFERRED_RENDERERS); i < len; i++) {
            RendererType preferredType = PREFERRED_RENDERERS[i];
            if (supportedRenderers.contains(preferredType)) {
                rendererType = preferredType;
                break;
            }
        }
    }

    // Convert RendererType enum to SDL renderer name
    const char* rendererName;
    switch (rendererType) {
        case RendererType::Vulkan:     rendererName = "vulkan"; break;
        case RendererType::Direct3d11: rendererName = "direct3d11"; break;
        case RendererType::Direct3d12: rendererName = "direct3d12"; break;
        case RendererType::Metal:      rendererName = "metal"; break;
        case RendererType::OpenGl:     rendererName = "opengl"; break;
        case RendererType::OpenGlEs:   rendererName = "opengles2"; break;
        case RendererType::Software:   rendererName = "software"; break;
        default:                       rendererName = nullptr; break;
    }

    // Create the renderer
    m_renderer = SDL_CreateRenderer(m_parentApp.getWindow(), rendererName);
    if (!m_renderer) {
        throw GmiException(std::string{"Failed to create renderer: "} + SDL_GetError());
    }

    m_rendererType = internal::sdlRendererNameToType(rendererName);
}

void SdlBackend::setVsync(const bool vsync) {
    SDL_SetRenderVSync(m_renderer, vsync ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED);
}

Texture& SdlBackend::loadTexture(const std::string& filename) {
    SDL_Surface* surface{SDL_LoadPNG(filename.c_str())};
    if (!surface) {
        throw GmiException(std::string{"Failed to load texture: "} + SDL_GetError());
    }

    SDL_Texture* rawTexture{SDL_CreateTextureFromSurface(m_renderer, surface)};
    if (!rawTexture) {
        throw GmiException(std::string{"Failed to create texture from surface: "} + SDL_GetError());
    }

    m_textures.push_back(std::make_unique<SdlTexture>(rawTexture));
    return *m_textures.back().get();
}

void SdlBackend::setClearColor(const Color color) {
    SDL_SetRenderDrawColorFloat(m_renderer, color.r, color.g, color.b, color.a);
}

constexpr int VERTEX_STRIDE = sizeof(math::Vertex);

void SdlBackend::renderFrame() {
    SDL_RenderClear(m_renderer);

    for (auto& [affine, color, texture, rawVertices] : m_queue) {
        auto vertices{*rawVertices};
        const size_t numVertices = vertices.size();
        for (int i = 0; i < numVertices; i++) {
            math::Vec2 n = math::affineApply(affine, math::Vec2{vertices[i].x, vertices[i].y});
            auto [x, y] = n;
            vertices[i].x = x;
            vertices[i].y = y;
        }
        const auto [r, g, b, a] = color;
        const SDL_FColor fColor{r, g, b, a};

        const auto rawTexture = static_cast<SDL_Texture*>(texture != nullptr ? texture->getRawTexture() : nullptr);
        SDL_RenderGeometryRaw(
            m_renderer,
            rawTexture,
            &vertices[0].x,
            VERTEX_STRIDE,
            &fColor,
            0,
            &vertices[0].u,
            VERTEX_STRIDE,
            static_cast<int>(numVertices),
            nullptr,
            0,
            4
        );
    }
    m_queue.clear();

    SDL_RenderPresent(m_renderer);
}


}
