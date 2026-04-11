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

namespace Aircraft {
Aircraft::Aircraft() {}

int Aircraft::init(const std::string& url) {
    Mav::ConnectionResult connectionResult =
        this->mavsdk.add_any_connection(url);
    if (connectionResult != Mav::ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connectionResult << '\n';
        return 1;
    }

    this->system =
        this->mavsdk.first_autopilot(Aircraft::CONNECTION_TIMEOUT).value();

    if (!this->system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }
    
    this->telemetry = std::make_shared<Mav::Telemetry>(system);
    this->action = std::make_shared<Mav::Action>(system);
    this->gimbal = std::make_shared<Mav::Gimbal>(system);
    this->mavlinkPassthrough =
        std::make_shared<Mav::MavlinkPassthrough>(this->system);

    const auto setRateResult =
        this->telemetry->set_rate_position(Aircraft::TELEMETRY_RATE);
    if (setRateResult != Mav::Telemetry::Result::Success) {
        std::cerr << "Setting rate failed: " << setRateResult << '\n';
        return 1;
    }

    return 0;
}

void Aircraft::subscribe() {
    this->gimbal->subscribe_attitude([&](Mav::Gimbal::Attitude attitude) {
        this->data.gimbal.pitch = attitude.euler_angle_forward.pitch_deg;
        this->data.gimbal.yaw = attitude.euler_angle_forward.yaw_deg;
        this->data.gimbal.roll = attitude.euler_angle_forward.roll_deg;
    });

    this->telemetry->subscribe_battery(
        [&](const Mav::Telemetry::Battery& battery) {
            this->data.battery.temperature = battery.temperature_degc;
            this->data.battery.voltage = battery.voltage_v;
            this->data.battery.current = battery.current_battery_a;
            this->data.battery.capacityConsumed = battery.capacity_consumed_ah;
            this->data.battery.remaining = battery.remaining_percent;
            this->data.battery.runtime = battery.time_remaining_s;
        }
    );
    this->telemetry->subscribe_position(
        [&](const Mav::Telemetry::Position& position) {
            this->data.latitude = position.latitude_deg;
            this->data.longitude = position.longitude_deg;
            this->data.altitude = position.relative_altitude_m;
            this->data.absoluteAltitude = position.absolute_altitude_m;
            this->data.lastUpdated =
                std::chrono::time_point_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now()
                );
        }
    );
    this->mavlinkPassthrough->subscribe_message(MAVLINK_MSG_ID_SYS_STATUS,
        [&](const mavlink_message_t& message) {
            mavlink_sys_status_t status;

            mavlink_msg_sys_status_decode(&message, &status);

            this->data.system.cpu = static_cast<float>(status.load) / 10000;
        });
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

int Aircraft::gotoLocation(Transform position, bool async) {
    auto [latitude, longitude, altitude, yaw] = position;
    if (async) {
        this->action->goto_location_async(latitude,
            longitude,
            altitude,
            yaw,
            [&](Mav::Action::Result result) { return 0; });
    }
    ACTION(this->action->goto_location(latitude, longitude, altitude, yaw),
        "Goto location failed: ");
}

} // namespace Aircraft
