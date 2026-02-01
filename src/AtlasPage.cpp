#include "PECT/AtlasPage.h"

namespace PECT
{
    std::optional<AtlasPos> AtlasPage::AddTexture(const std::string& name, ImageData& data) noexcept
    {
        if (m_TextureEntries.empty())
        {
            m_TextureEntries.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(std::move(data), AtlasPos(0,0)));
            return AtlasPos(0,0);
        }

        AtlasInt x = 0, y = 0;
        bool isSpace = true;
        
        while (isSpace)
        {
            bool collision = false;

            for (const auto& [key,val] : m_TextureEntries)
            {
                if (val.WouldCollide(x, y, data.GetWidth(), data.GetHeight()))
                {
                    collision = true;
                    break;
                }
            }

            if (!collision)
            {
                m_TextureEntries.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(std::move(data), AtlasPos(x,y)));
                return AtlasPos(x,y);
            }

            ++x;

            if (x == m_Width - 1 - data.GetHeight())
            {
                ++y;
                x = 0;

                if (y == m_Height - 1 - data.GetHeight())
                {
                    isSpace = false;
                }
            }
        }

        return {};
    }

    bool AtlasPage::RemoveTexture(const std::string& name) noexcept
    {
        auto it = m_TextureEntries.begin();

        while (it != m_TextureEntries.end())
        {
            if (it->first == name)
            {
                m_TextureEntries.erase(it);
                return true;
            }
        }

        return false;
    }

    // bool AtlasPage::AddFontTexture(const std::string& name, std::uint16_t w, std::uint16_t h, std::shared_ptr<char[]> d, std::uint32_t fontIndex, std::uint8_t code, std::int32_t bearingX, std::int32_t bearingY, std::int32_t advance)
    // {
    //     if (m_Textures.empty())
    //     {
    //         m_Textures.push_back(std::make_shared<PageTexture>(name, 0, 0, w, h, d, code, bearingX, bearingY, advance));
    //         return true;
    //     }

    //     if (w == 0 || h == 0)
    //     {
    //         m_Textures.push_back(std::make_shared<PageTexture>(name, 0, 0, w, h, d, code, bearingX, bearingY, advance));
    //         return true;
    //     }

    //     std::uint16_t x = 0, y = 0;
    //     bool fits = false;
    //     std::uint16_t gap = 1;

    //     while (!fits)
    //     {
    //         if (Collides(x, y, w, h, gap))
    //         {
    //             ++x;

    //             if (x == m_Width - w + 1)
    //             {
    //                 x = 0;
    //                 ++y;

    //                 if (y == m_Height - h + 1)
    //                 {
    //                     return false;
    //                 }
    //             }
    //         }
    //         else
    //         {
    //             fits = true;
    //         }
    //     }

    //     m_Textures.push_back(std::make_shared<PageTexture>(name, x, y, w, h, d, code, bearingX, bearingY, advance));
    //     return true;
    // }
}