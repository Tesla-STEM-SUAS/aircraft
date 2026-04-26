/*
COPYRIGHT (C) 2026 SUAS@STEM

ALL RIGHTS RESERVED. UNAUTHORIZED COPYING, MODIFICATION, DISTRIBUTION, OR USE
OF THIS SOFTWARE WITHOUT PRIOR PERMISSION IS STRICTLY PROHIBITED.

THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM,
OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

AUTHOR: ETHAN CHAN
*/

#pragma once

#include <string>

#include "json.hpp"

namespace Aircraft {
class Settings {
  private:
    nlohmann::json data;
    std::string filepath;

    static nlohmann::json::json_pointer _toJsonPointer(const std::string& key) {
        std::string path;
        path.reserve(key.size() + 1);
        path.push_back('/');

        for (char c : key) path.push_back(c == '.' ? '/' : c);

        return nlohmann::json::json_pointer(path);
    }

    static std::optional<std::reference_wrapper<nlohmann::json>>
    _resolve(nlohmann::json& root, const std::string& key) {
        const nlohmann::json::json_pointer pointer = _toJsonPointer(key);
        if (!root.contains(pointer)) return std::nullopt;

        return std::ref(root.at(pointer));
    }

    static std::optional<std::reference_wrapper<const nlohmann::json>>
    _resolve(const nlohmann::json& root, const std::string& key) {
        const nlohmann::json::json_pointer pointer = _toJsonPointer(key);
        if (!root.contains(pointer)) return std::nullopt;

        return std::cref(root.at(pointer));
    }

  public:
    Settings();

    void load();
    void save() const;

    /*
    Obtain a value from the settings configuration, specified in
    Settings::filepath.

    The key is a dot-separated string representing the path to the JSON object.
    For example, "camera.rtsp_url" would access the "rtsp_url" field within the
    "camera" object.
    */
    template <typename T> T get(const std::string& key, bool& exists = false) const {
        const nlohmann::json::json_pointer pointer = _toJsonPointer(key);
        exists = data.contains(pointer);
        if (exists) return data.at(pointer).template get<T>();

        return T();
    }

    template <typename T>
    T get(const std::string& key, const T& defaultValue) const {
        const nlohmann::json::json_pointer pointer = _toJsonPointer(key);
        try {
            if (data.contains(pointer))
                return data.at(pointer).template get<T>();
        } catch (const nlohmann::json::exception&) {
        }
        return defaultValue;
    }
    template <typename T> void set(const std::string& key, const T& value) {
        data[_toJsonPointer(key)] = value;
    }
};

extern Settings settings;
} // namespace Aircraft
