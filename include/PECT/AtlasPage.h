#ifndef PECT_ATLASPAGE_H_
#define PECT_ATLASPAGE_H_

#include <cstdint>
#include <memory>
#include <vector>
#include <utility>
#include <unordered_map>
#include <optional>
#include <iostream>

#include "PECT/ContentData.h"

namespace PECT
{
    using AtlasInt = std::int16_t;
    constexpr AtlasInt g_TextureGap = 1;

    class AtlasPos
    {
    private:
        AtlasInt m_X, m_Y;
    public:
        AtlasPos(AtlasInt x, AtlasInt y) noexcept
        : m_X(x), m_Y(y) { }

        AtlasPos(const AtlasPos& other) noexcept
        : m_X(other.m_X), m_Y(other.m_Y) { }

        AtlasPos(AtlasPos&& other) noexcept
        : m_X(other.m_X), m_Y(other.m_Y) { }

        AtlasInt GetX() const noexcept { return m_X; }
        AtlasInt GetY() const noexcept { return m_Y; }
    };
    
    class AtlasTextureEntry
    {
    private:
        ImageData m_ImageData;
        AtlasPos m_Pos;
    public:
        AtlasTextureEntry(ImageData data, AtlasPos pos) noexcept
        : m_ImageData(std::move(data)), m_Pos(std::move(pos)) {}

        const ImageData& GetImageData() const noexcept { return m_ImageData; }
        const AtlasPos& GetPos() const noexcept { return m_Pos; }

        bool WouldCollide(AtlasInt x, AtlasInt y, AtlasInt w, AtlasInt h) const noexcept
        {
            AtlasInt gap = g_TextureGap;
            return x < m_Pos.GetX() + m_ImageData.GetWidth() + gap &&
                   m_Pos.GetX() - gap < x + w &&
                   y < m_Pos.GetY() + m_ImageData.GetHeight() + gap &&
                   m_Pos.GetY() - gap < y + h;
        }
    };

    class AtlasFontChar
    {
    private:
        
    };

    class AtlasFontEntry
    {
    private:
        
    };

    class AtlasPage
    {
        friend class ContentFile;
    private:
        AtlasInt m_Width, m_Height;
        std::unordered_map<std::string, AtlasTextureEntry> m_TextureEntries;
        //std::unordered_map<std::string, AtlasFontEntry> m_FontEntries;

        //bool Collides(std::uint16_t, std::uint16_t, std::uint16_t, std::uint16_t, std::uint16_t);

        
    public:
        //static AtlasInt m_TextureGap; // 1

        AtlasPage(AtlasInt w, AtlasInt h) : m_Width(w), m_Height(h) { }

        AtlasInt GetWidth() const noexcept { return m_Width; }
        AtlasInt GetHeight() const noexcept { return m_Height; }

        bool HasName(const std::string& name) const noexcept
        {
            for (const auto& [n,e] : m_TextureEntries)
            {
                if (name == n) { return true; }
            }

            return false;
        }

        std::optional<AtlasPos> AddTexture(const std::string&, ImageData&) noexcept;
        bool RemoveTexture(const std::string&) noexcept;
        //bool AddFontTexture(const std::string&, std::uint16_t, std::uint16_t, const unsigned char* const, std::uint32_t, std::uint8_t, std::int32_t, std::int32_t, std::int32_t);

        const std::unordered_map<std::string, AtlasTextureEntry>& GetTextures() const noexcept { return m_TextureEntries; }
        //temp
        //inline std::vector<PageTexture>& GetPageTextures() { return m_Textures; }
    };
}

#endif