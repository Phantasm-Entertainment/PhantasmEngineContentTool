#ifndef PECT_CONTENTLOADER_H_
#define PECT_CONTENTLOADER_H_

#include <expected>
#include <string>

#include <zlib.h>
#include <png.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "PECT/ContentData.h"

namespace PECT
{
    class ContentLoader
    {
    private:
        ContentLoader() { }
    public:
        static std::expected<ImageData, std::string> LoadPNG(const std::string&);
        static std::expected<FontData, std::string> LoadFont(const std::string&, std::uint16_t);
    };
}

#endif