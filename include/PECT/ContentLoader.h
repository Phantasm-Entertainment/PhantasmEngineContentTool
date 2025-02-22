#ifndef PECT_CONTENTLOADER_H_
#define PECT_CONTENTLOADER_H_

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

#include <zlib.h>
#include <png.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "PECT/FontData.h"

namespace PECT
{
    class ContentLoader
    {
    private:
        inline ContentLoader() { }
    public:
        static std::shared_ptr<char[]> LoadPNG(const std::string&, std::size_t*, std::size_t*);
        static std::shared_ptr<FontData> LoadFont(const std::string&, std::uint16_t);
    };
}

#endif