#include "HttpClient.hpp"

HttpClient::HttpClient(const std::string& base_url, const std::string& user_agent)
    : user_agent_(user_agent)
    , client_(base_url)
{
    client_.set_connection_timeout(kTimeoutSec);
    client_.set_read_timeout(kTimeoutSec);
    client_.set_follow_location(true);
}

std::optional<HttpResponse> HttpClient::get(const std::string& path,
                                             const httplib::Headers& extra_headers)
{
    httplib::Headers headers = extra_headers;
    headers.emplace("User-Agent", user_agent_);

    for (int attempt = 0; attempt <= kRetries; ++attempt) {
        auto res = client_.Get(path, headers);
        if (res) {
            return HttpResponse{res->status, res->body};
        }
    }
    return std::nullopt;
}

std::optional<std::vector<uint8_t>> HttpClient::get_binary(const std::string& path) {
    httplib::Headers headers;
    headers.emplace("User-Agent", user_agent_);

    for (int attempt = 0; attempt <= kRetries; ++attempt) {
        auto res = client_.Get(path, headers);
        if (res && res->status == 200) {
            const auto& body = res->body;
            return std::vector<uint8_t>(body.begin(), body.end());
        }
    }
    return std::nullopt;
}
