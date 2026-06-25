#include "CoverArtClient.hpp"
#include "HttpClient.hpp"

std::optional<std::vector<uint8_t>> CoverArtClient::download_front(
    const std::string& release_mbid,
    const std::string& size)
{
    // CAA URL pattern: /release/{mbid}/front-{size}
    // The server redirects to the actual image; HttpClient follows redirects.
    std::string path = "/release/" + release_mbid + "/front-" + size;

    HttpClient http(kBaseUrl, "MusicTagCLI/0.1");
    return http.get_binary(path);
}
