#include "TagWriter.hpp"

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tfile.h>
#include <taglib/tvariant.h>
#include <taglib/tpropertymap.h>

static TagLib::String to_tstring(const std::string& s) {
    return TagLib::String(s, TagLib::String::UTF8);
}

static bool write_impl(const TagData& td,
                       const std::filesystem::path& path,
                       const std::set<std::string>* fields) {
    TagLib::FileRef f(path.c_str());
    if (f.isNull() || !f.tag()) return false;

    TagLib::Tag* tag = f.tag();
    auto want = [&](const char* name) {
        return fields == nullptr || fields->count(name) > 0;
    };

    if (want("title"))   tag->setTitle(to_tstring(td.title));
    if (want("artist"))  tag->setArtist(to_tstring(td.artist));
    if (want("album"))   tag->setAlbum(to_tstring(td.album));
    if (want("genre"))   tag->setGenre(to_tstring(td.genre));
    if (want("comment")) tag->setComment(to_tstring(td.comment));
    if (want("year"))    tag->setYear(td.year);
    if (want("track"))   tag->setTrack(td.track);

    // Cover art via TagLib 2.x complex properties
    if (want("cover") && f.file()) {
        if (td.cover_bytes.empty()) {
            f.file()->setComplexProperties("PICTURE", {});
        } else {
            TagLib::ByteVector bv(
                reinterpret_cast<const char*>(td.cover_bytes.data()),
                static_cast<unsigned int>(td.cover_bytes.size()));

            TagLib::VariantMap pic;
            pic.insert("data",        TagLib::Variant(bv));
            pic.insert("mimeType",    TagLib::Variant(to_tstring(td.cover_mime)));
            pic.insert("pictureType", TagLib::Variant(3)); // 3 = Front cover
            pic.insert("description", TagLib::Variant(TagLib::String()));

            f.file()->setComplexProperties("PICTURE", {pic});
        }
    }

    return f.save();
}

bool TagWriter::write(const TagData& td, const std::filesystem::path& path) {
    return write_impl(td, path, nullptr);
}

BatchResult TagWriter::write_batch(
    const std::vector<std::filesystem::path>& paths,
    const TagData& td,
    const std::set<std::string>& fields)
{
    BatchResult result;
    for (const auto& p : paths) {
        if (write_impl(td, p, &fields))
            ++result.ok;
        else
            ++result.error;
    }
    return result;
}
