#pragma once

#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <hardware_interface/base_interface.hpp>
#include <hardware_interface/system_interface.hpp>
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <hardware_interface/types/hardware_interface_status_values.hpp>

#include <fri/friLBRClient.h>
#include <fri/friUdpConnection.h>
#include <fri/friClientApplication.h>


namespace LBR {

class FRIHardwareInterface : public hardware_interface::BaseInterface<hardware_interface::SystemInterface>, public KUKA::FRI::LBRClient {

    public:
        FRIHardwareInterface() : app_(connection_, *this) { };
        ~FRIHardwareInterface() = default;

        // hardware interface
        hardware_interface::return_type configure(const hardware_interface::HardwareInfo& system_info) override;  // check ros2 control and set status
        std::vector<hardware_interface::StateInterface> export_state_interfaces() override;
        std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

        hardware_interface::return_type prepare_command_mode_switch(const std::vector<std::string>& start_interfaces, const std::vector<std::string>& stop_interfaces) override;  // not supported in FRI

        hardware_interface::return_type start() override;
        hardware_interface::return_type stop() override;

        hardware_interface::return_type read() override;
        hardware_interface::return_type write() override;

        // FRI
        void onStateChange(KUKA::FRI::ESessionState old_state, KUKA::FRI::ESessionState new_state) override;
        void command() override;

    private:
        std::string FRI_HW_LOGGER = "FRIHardwareInterface";

        // exposed states
        std::vector<double> hw_position_;      // accessible through FRI
        std::vector<double> hw_effort_;        // accessible through FRI

        // commands
        std::vector<double> hw_position_command_;  // supported by FRI
        std::vector<double> hw_effort_command_;    // supported by FRI

        // FRI
        KUKA::FRI::UdpConnection connection_;
        KUKA::FRI::ClientApplication app_;

        std::uint16_t hw_port_;
        const char* hw_remote_host_;

        // track command mode as FRI does not support switches
        bool command_mode_init_;

        std::string fri_e_session_state_to_string_(const KUKA::FRI::ESessionState& state);
};

} // end of name space LBR
