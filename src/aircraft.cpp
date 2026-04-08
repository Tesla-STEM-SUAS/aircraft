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

#include "aircraft.hpp"

#include <chrono>
#include <iostream>
#include <thread>

Aircraft::Aircraft() {}

int Aircraft::init(const std::string& url) {
    ConnectionResult connectionResult = this->mavsdk.add_any_connection(url);
    if (connectionResult != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connectionResult << '\n';
        return 1;
    }

    auto system = this->mavsdk.first_autopilot(Aircraft::CONNECTION_TIMEOUT);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    this->telemetry = std::make_shared<Telemetry>(system.value());
    this->action = std::make_shared<Action>(system.value());

    const auto setRateResult =
        this->telemetry->set_rate_position(Aircraft::TELEMETRY_RATE);
    if (setRateResult != Telemetry::Result::Success) {
        std::cerr << "Setting rate failed: " << setRateResult << '\n';
        return 1;
    }

    this->telemetry->subscribe_position(
        [&](const Telemetry::Position& position) {
            this->data.latitude = position.latitude_deg;
            this->data.longitude = position.longitude_deg;
            this->data.altitude = position.relative_altitude_m;
            this->data.absoluteAltitude = position.absolute_altitude_m;
        }
    );

    return 0;
}

void Aircraft::readyToArm() {
    while (!this->telemetry->health_all_ok())
        std::this_thread::sleep_for(std::chrono::seconds(1));
}

int Aircraft::arm() {
    ACTION(this->action->arm(), "Arming failed: ");
}

int Aircraft::armForce() {
    ACTION(this->action->arm_force(), "Force-arming failed: ");
}

int Aircraft::disarm() {
    ACTION(this->action->disarm(), "Disarming failed: ");
}

int Aircraft::takeOff() {
    ACTION(this->action->takeoff(), "Takeoff failed: ");
}

int Aircraft::land() {
    ACTION(this->action->land(), "Landing failed: ");
}

int Aircraft::gotoLocation(Pos3Y position, bool async) {
    auto [latitude, longitude, altitude, yaw] = position;
    if (async) {
        this->action->goto_location_async(
            latitude, longitude, altitude, yaw, [&](Action::Result result) {
                return 0;
            }
        );
    }
    ACTION(this->action->goto_location(latitude, longitude, altitude, yaw),
        "Goto location failed: ");
}
