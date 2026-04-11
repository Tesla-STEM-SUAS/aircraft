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
#include <mavsdk/plugins/gimbal/gimbal.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

#include <chrono>

namespace Mav = mavsdk;

namespace Aircraft {

/*
MAVLink system information
*/
struct System {
    // CPU utilization (0-1), obtained from system mavlink passthrough
    float cpu;
};

struct Gimbal {
    float pitch;
    float yaw;
    float roll;
};

struct AircraftData {
    double latitude;
    double longitude;
    float altitude;
    float absoluteAltitude;

    float batterySoc;

    // Battery information, obtained from telemetry
    struct Battery {
        // Temperature of battery, in Celsius
        float temperature;

        // Voltage of battery in volts
        float voltage;

        // Current of battery in amperes
        float current;

        // Consumed battery charge in amp-hours
        float capacityConsumed;

        // Remaining battery capacity (0-100)
        float remaining;

        // Remaining battery duration, in seconds
        float runtime;
    };
    Battery battery;
    Gimbal gimbal;
    System system;

    std::chrono::time_point<std::chrono::system_clock,
        std::chrono::milliseconds>
        lastUpdated;
};

#define ACTION(expression, message)                                            \
    do {                                                                       \
        const Mav::Action::Result result = (expression);                       \
        if (result != Mav::Action::Result::Success) {                          \
            std::cout << message << result << '\n';                            \
            return 1;                                                          \
        }                                                                      \
        return 0;                                                              \
    } while (0)

struct Vec3 {
    float x;
    float y;
    float z;
};

struct Transform {
    double latitude;
    double longitude;
    float altitude; // m above sea level
    float yaw;      // 0 is North, positive is clockwise
};

class Aircraft {
  private:
    std::shared_ptr<Mav::System> system;
    std::shared_ptr<Mav::Action> action;
    std::shared_ptr<Mav::Telemetry> telemetry;
    std::shared_ptr<Mav::MavlinkPassthrough> mavlinkPassthrough;
    std::shared_ptr<Mav::Gimbal> gimbal;

  public:
    static constexpr double CONNECTION_TIMEOUT = 3.0;
    static constexpr double TELEMETRY_RATE = 1.0;

    Mav::Mavsdk mavsdk{
        Mav::Mavsdk::Configuration{Mav::ComponentType::GroundStation}
    };

    AircraftData data;

    Aircraft();
    int init(const std::string& url);
    void subscribe();

    // Blocking, waits until the aircraft is ready to arm
    void readyToArm();

    int arm();
    int armForce();
    int disarm();

    int takeOff();
    int land();

    int gotoLocation(Transform position, bool async = true);
};

} // namespace Aircraft
