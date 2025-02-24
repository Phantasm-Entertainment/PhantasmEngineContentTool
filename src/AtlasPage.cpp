#include "PECT/AtlasPage.h"

namespace PECT
{
    // true if it collides with something, false if it fits
    bool AtlasPage::Collides(std::uint16_t startX, std::uint16_t startY, std::uint16_t startW, std::uint16_t startH, std::uint16_t gap)
    {
        if (m_Textures.empty())
        {
            return false;
        }

        std::uint16_t x = startX == 0 ? 0 : startX - gap;
        std::uint16_t y = startY == 0 ? 0 : startY - gap;
        std::uint16_t w = startW + gap * 2, h = startH + gap * 2;

        for (const std::shared_ptr<PageTexture>& pageTexture : m_Textures)
        {
            if (x < (pageTexture->X + pageTexture->Width) &&
                pageTexture->X < (x + w) &&
                y < (pageTexture->Y + pageTexture->Height) &&
                pageTexture->Y < (y + h))
            {
                return true;
            }
        }

        return false;
    }

    // return true if found spot
    bool AtlasPage::AddTexture(const std::string& name, std::uint16_t w, std::uint16_t h, std::shared_ptr<char[]> d)
    {
        std::shared_ptr<PageTexture> pt;

        if (m_Textures.empty())
        {
            m_Textures.push_back(std::make_shared<PageTexture>(name, 0, 0, w, h, d));
            return true;
        }

        std::uint16_t x = 0, y = 0;
        bool fits = false;
        std::uint16_t gap = 1;

        while (!fits)
        {
            if (Collides(x, y, w, h, gap))
            {
                ++x;

                if (x == m_Width - w + 1)
                {
                    x = 0;
                    ++y;

                    if (y == m_Height - h + 1)
                    {
                        return false;
                    }
                }
            }
            else
            {
                fits = true;
            }
        }

        m_Textures.push_back(std::make_shared<PageTexture>(name, x, y, w, h, d));
        return true;
    }

    bool AtlasPage::AddFontTexture(const std::string& name, std::uint16_t w, std::uint16_t h, std::shared_ptr<char[]> d, std::uint32_t fontIndex, std::uint8_t code, std::int32_t bearingX, std::int32_t bearingY, std::int32_t advance)
    {
        if (m_Textures.empty())
        {
            m_Textures.push_back(std::make_shared<PageTexture>(name, 0, 0, w, h, d, code, bearingX, bearingY, advance));
            return true;
        }

        if (w == 0 || h == 0)
        {
            m_Textures.push_back(std::make_shared<PageTexture>(name, 0, 0, w, h, d, code, bearingX, bearingY, advance));
            return true;
        }

        std::uint16_t x = 0, y = 0;
        bool fits = false;
        std::uint16_t gap = 1;

        while (!fits)
        {
            if (Collides(x, y, w, h, gap))
            {
                ++x;

                if (x == m_Width - w + 1)
                {
                    x = 0;
                    ++y;

                    if (y == m_Height - h + 1)
                    {
                        return false;
                    }
                }
            }
            else
            {
                fits = true;
            }
        }

        m_Textures.push_back(std::make_shared<PageTexture>(name, x, y, w, h, d, code, bearingX, bearingY, advance));
        return true;
    }
}