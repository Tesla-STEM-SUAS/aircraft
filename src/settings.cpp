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

#include "settings.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

namespace Aircraft {
Settings::Settings() {
    #pragma warning(suppress : 4996)
    const char* userProfile = std::getenv("USERPROFILE");
    std::filesystem::path dir =
        userProfile != nullptr
            ? std::filesystem::path(userProfile) / "AppData" / "Local" /
                  "aircraft"
            : std::filesystem::path("AppData") / "Local" / "aircraft";

    std::filesystem::create_directories(dir);

    this->filepath = (dir / "config.json").string();
    this->load();
}

void Settings::load() {
    std::ifstream file(this->filepath);
    if (file.is_open()) {
        file >> data;
        std::cout << "Loaded configuration file at " << this->filepath << '\n';
    } else {
        data = nlohmann::json::object();
        std::cout << "No configuration file was found, creating new one at "
                  << this->filepath << '\n';
    }
}

void Settings::save() const {
    std::ofstream file(this->filepath);
    file << data.dump(4);
}

Settings settings;
} // namespace Aircraft
