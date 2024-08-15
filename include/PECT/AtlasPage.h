#ifndef PECT_ATLASPAGE_H_
#define PECT_ATLASPAGE_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "PECT/PageTexture.h"

namespace PECT
{
    class AtlasPage
    {
        friend class ContentFile;
    private:
        std::uint16_t m_Width, m_Height;
        std::vector<std::shared_ptr<PageTexture>> m_Textures;

        bool Collides(std::uint16_t, std::uint16_t, std::uint16_t, std::uint16_t, std::uint16_t);
    public:
        inline AtlasPage(std::uint16_t w, std::uint16_t h) : m_Width(w), m_Height(h) { }

        inline std::uint16_t GetWidth() const noexcept { return m_Width; }
        inline std::uint16_t GetHeight() const noexcept { return m_Height; }

        bool AddTexture(const std::string&, std::uint16_t, std::uint16_t, std::shared_ptr<std::uint8_t[]>);
        bool AddFontTexture(const std::string&, std::uint16_t, std::uint16_t, std::shared_ptr<std::uint8_t[]>, std::uint32_t, std::uint8_t, std::int32_t, std::int32_t, std::int32_t);

        //temp
        inline std::vector<std::shared_ptr<PageTexture>>& GetPageTextures() { return m_Textures; }
    };
}

#endif