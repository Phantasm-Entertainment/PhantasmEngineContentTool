#ifndef PECT_CONTENTFILE_H_
#define PECT_CONTENTFILE_H_

#include <cstdint>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <memory>
#include <fstream>

#include "PECT/AtlasPage.h"
#include "PECT/FontData.h"

namespace PECT
{
    struct AtlasFontEntry
    {
        std::string Name;
        std::uint16_t Ascender, Descender, LineSpacing;

        inline AtlasFontEntry(const std::string& n, std::uint16_t a, std::uint16_t d, std::uint16_t l)
        : Name(n), Ascender(a), Descender(d), LineSpacing(l) { }
    };

    class ContentFile
    {
    private:
        static const char m_FileHeader[8];
        static std::uint32_t m_FileVersion;

        static std::uint32_t ReadUInt32(char*);
        static std::uint16_t ReadUInt16(char*);

        static void WriteUInt32(std::uint32_t, char*);
        static void WriteUInt16(std::uint16_t, char*);

        // the good stuff

        std::uint16_t m_PageSize;
        std::vector<std::shared_ptr<AtlasPage>> m_Pages;
        std::vector<AtlasFontEntry> m_FontEntries;

        void CheckName(const std::string&);

        void AddFontTexture(const std::string&, FontCharData&, std::uint32_t);
    public:
        static std::shared_ptr<ContentFile> LoadFromFile(const std::string&);

        inline std::vector<std::shared_ptr<AtlasPage>>& GetPages() { return m_Pages; }
        inline std::vector<AtlasFontEntry>& GetFontEntries() { return m_FontEntries; }
        inline std::uint16_t GetPageSize() const { return m_PageSize; }

        ContentFile();
        ~ContentFile();

        void AddTexture(const std::string&, std::uint16_t, std::uint16_t, std::shared_ptr<char[]>);
        bool RemoveTexture(const std::string&);
        void AddFont(const std::string&, std::shared_ptr<FontData>);
        bool RemoveFont(const std::string&);
        void SaveToFile(const std::string&);
    };
}

#endif

/*

FILE FORMAT

Header (8 bytes)
Version (4 bytes)
Pages (2 bytes)

FOREACH Page
    Texture list size (4 bytes)
    
END


*/
