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

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

using namespace mavsdk;

struct AircraftData {
    double latitude;
    double longitude;
    float altitude;
    float absoluteAltitude;

    float batterySoc;
};

#define ACTION(expression, message)                                            \
    do {                                                                       \
        const Action::Result result = (expression);                            \
        if (result != Action::Result::Success) {                               \
            std::cout << message << result << '\n';                            \
            return 1;                                                          \
        }                                                                      \
        return 0;                                                              \
    } while (0)

struct Pos3Y {
    double latitude;
    double longitude;
    float altitude; // m above sea level
    float yaw;      // 0 is North, positive is clockwise
};

class Aircraft {
  private:
    std::shared_ptr<System> system;
    std::shared_ptr<Action> action;
    std::shared_ptr<Telemetry> telemetry;

  public:
    static constexpr double CONNECTION_TIMEOUT = 3.0;
    static constexpr double TELEMETRY_RATE = 1.0;

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    AircraftData data;

    Aircraft();
    int init(const std::string& url);

    // Blocking, waits until the aircraft is ready to arm
    void readyToArm();
    int arm();
    int armForce();
    int disarm();

    int takeOff();
    int land();

    int gotoLocation(Pos3Y position, bool async = true);
};
