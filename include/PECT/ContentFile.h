#ifndef PECT_CONTENTFILE_H_
#define PECT_CONTENTFILE_H_

#include <cstdint>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <memory>
#include <fstream>
#include <optional>
#include <expected>
#include <variant>

#include "PECT/AtlasPage.h"
//#include "PECT/FontData.h"

namespace PECT
{
    // struct AtlasFontEntry
    // {
    //     std::string Name;
    //     std::uint16_t Ascender, Descender, LineSpacing;

    //     inline AtlasFontEntry(const std::string& n, std::uint16_t a, std::uint16_t d, std::uint16_t l)
    //     : Name(n), Ascender(a), Descender(d), LineSpacing(l) { }
    // };

    class SaveResult
    {
    private:
        std::variant<std::monostate, bool, std::string> m_Result;
    public:
        SaveResult() noexcept { }
        SaveResult(bool b) noexcept : m_Result(b) { }
        SaveResult(const std::string& msg) noexcept : m_Result(msg) { }
        
        bool IsSuccess() const noexcept { return m_Result.index() == 0; }
        bool NeedsPath() const noexcept { return m_Result.index() == 1; }
        const std::string& GetError() const noexcept { return std::get<2>(m_Result); }
    };

    class ContentFile
    {
    private:
        static const unsigned char m_FileHeader[8]; // { 80, 69, 67, 70, 2, 3, 1, 7 }
        static const std::uint32_t m_FileVersion; // 1
        static const std::uint16_t m_MaxPageSize; // 4096

        // integer to bytes conversions
        void ReadUInt32(std::uint32_t* val, unsigned char* buffer)
        {
            *val = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);
        }

        void ReadUInt16(std::uint32_t* val, unsigned char* buffer)
        {
            *val = (buffer[0] << 8) | (buffer[1]);
        }

        void WriteUInt32(std::uint32_t val, unsigned char* buf)
        {
            buf[0] = (val >> 24) & 0xFF;
            buf[1] = (val >> 16) & 0xFF;
            buf[2] = (val >> 8) & 0xFF;
            buf[3] = val & 0xFF;
        }

        void WriteUInt16(std::uint16_t val, unsigned char* buf)
        {
            buf[0] = (val >> 8) & 0xFF;
            buf[1] = val & 0xFF;
        }

        // data
        std::optional<std::string> m_SavePath;
        std::vector<AtlasPage> m_Pages;
        //std::vector<AtlasFontEntry> m_FontEntries;

        //void AddFontTexture(const std::string&, FontCharData&, std::uint32_t);
    public:
        ~ContentFile() { Save(); }

        bool IsNameTaken(const std::string& name) const noexcept
        {
            for (const auto& page : m_Pages)
            {
                if (page.HasName(name)) { return true; }
            }

            return false;
        }

        std::vector<AtlasPage>& GetPages() noexcept { return m_Pages; }
        //inline std::vector<AtlasFontEntry>& GetFontEntries() noexcept { return m_FontEntries; }

        std::expected<AtlasPos, std::string> AddTexture(const std::string&, ImageData&);
        bool RemoveTexture(const std::string&);
        //void AddFont(const std::string&, const FontData&);
        //bool RemoveFont(const std::string&);

        SaveResult Save(std::string path = "");
        std::expected<void, std::string> Load(const std::string&);
    };
}

#endif