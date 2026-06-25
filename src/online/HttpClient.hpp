#pragma once
#include <string>
#include <optional>

// HTTPS support — requires OpenSSL (compile definition set in CMakeLists.txt)
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

struct HttpResponse {
    int         status = 0;
    std::string body;
    bool        ok() const { return status >= 200 && status < 300; }
};

class HttpClient {
public:
    explicit HttpClient(const std::string& base_url, const std::string& user_agent = "MusicTagCLI/0.1");

    // Synchronous GET with timeout and simple retry.
    // Returns nullopt on connection failure after retries.
    std::optional<HttpResponse> get(const std::string& path,
                                    const httplib::Headers& headers = {});

    // Download binary data (for cover art images).
    std::optional<std::vector<uint8_t>> get_binary(const std::string& path);

private:
    std::string      user_agent_;
    httplib::SSLClient client_;

    static constexpr int kTimeoutSec = 8;
    static constexpr int kRetries    = 2;
};
