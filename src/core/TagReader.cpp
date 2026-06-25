#include "TagReader.hpp"

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#include <taglib/toolkit/tpropertymap.h>

// TagLib 2.x complex properties (cover art)
#include <taglib/toolkit/tfile.h>
#include <taglib/toolkit/tvariant.h>

static std::string tstring_to_utf8(const TagLib::String& s) {
    return s.to8Bit(true);
}

std::optional<TagData> TagReader::read(const std::filesystem::path& path) {
    TagLib::FileRef f(path.c_str());
    if (f.isNull() || !f.tag()) return std::nullopt;

    TagData td;
    TagLib::Tag* tag = f.tag();

    td.title   = tstring_to_utf8(tag->title());
    td.artist  = tstring_to_utf8(tag->artist());
    td.album   = tstring_to_utf8(tag->album());
    td.genre   = tstring_to_utf8(tag->genre());
    td.comment = tstring_to_utf8(tag->comment());
    td.year    = tag->year();
    td.track   = tag->track();

    // Audio properties
    if (auto* ap = f.audioProperties()) {
        td.bitrate     = ap->bitrate();
        td.sample_rate = ap->sampleRate();
        td.channels    = ap->channels();
        td.length_sec  = ap->lengthInSeconds();
    }

    // Cover art via TagLib 2.x complex properties API
    if (f.file()) {
        auto pics = f.file()->complexProperties("PICTURE");
        if (!pics.isEmpty()) {
            const auto& pic = pics.front();
            auto it_data = pic.find("data");
            auto it_mime = pic.find("mimeType");
            if (it_data != pic.end()) {
                TagLib::ByteVector bv = (*it_data).second.toByteVector();
                td.cover_bytes.assign(
                    reinterpret_cast<const uint8_t*>(bv.data()),
                    reinterpret_cast<const uint8_t*>(bv.data()) + bv.size());
            }
            if (it_mime != pic.end()) {
                td.cover_mime = tstring_to_utf8((*it_mime).second.toString());
            }
        }
    }

    return td;
}
