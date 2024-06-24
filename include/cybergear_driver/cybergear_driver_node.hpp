#include <rclcpp/rclcpp.hpp>
#include "cybergear_driver/visivility_control.h"

namespace cybergear_driver{

class CyberGearDriver : public rclcpp::Node{
public:
    CYBERGEAR_DRIVER_PUBLIC
    explicit CyberGearDriver(const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

    CYBERGEAR_DRIVER_PUBLIC
    explicit CyberGearDriver(const std::string& name_space, const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

private:
    rclcpp::Subscription<std::msgs::msg::Empty>::SharedPtr _subscription_stop;
    rclcpp::Subscription<std::msgs::msg::Empty>::SharedPtr _subscription_restart;
    rclcpp::Subscription<socketcan_interface_msg::msg::SocketcanIF>::SharedPtr _subscription_emergency;

    enum class Mode{
        stay,
        stop,
        position,
        velocity
    }
};

}