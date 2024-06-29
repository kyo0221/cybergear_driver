#include "cybergear_driver/cybergear_driver_node.hpp"

using namespace utils;

namespace cybergear_driver{
CyberGearDriver::CyberGearDriver(const rclcpp::NodeOptions& options) : CyberGearDriver("", options) {}

CyberGearDriver::CyberGearDriver(const std::string& name_space, const rclcpp::NodeOptions& options)
: rclcpp::Node("cybergear_driver_node", name_space, options),
interval_ms(get_parameter("interval_ms").as_int()),
rotate_ratio(1.0 / get_parameter("gear_ratio").as_double()),
is_reverse_flag(get_parameter("reverse_flag").as_bool()),
{
    _subscription_stop = this->create_subscription<std::msgs::msg::Empty>(
        "stop",
        _qos,
        std::bind(&CyberGearDriver::_subscriber_callback_stop)
    );
    _subscription_restart = this->create_subscription<std_msgs::msg::Empty>(
        "restart",
        _qos,
        std::bind(&CyberGearDriver::_subscriber_callback_restart)
    );
    _subscription_position = this->cteate_subscription<cybergear_msgs::msg::CyberhearState>(
        "cybergear_state",
        _qos,
        std::bind(&CyberGearDriver::_subscriber_callback_position)
    )

    _subscription_rpm = this->create_subscription<socketcan_interface_msg::msg::SocketcanIF>(
        "can_rx_711",
        _qos,
        std::bind(&RoboteqDriver::_subscriber_callback_rpm, this, std::placeholders::_1)
    );
    _subscription_emergency = this->create_subscription<socketcan_interface_msg::msg::SocketcanIF>(
        "can_rx_712",
         _qos,
         std::bind(&CyberGearDriver::_subscription_emergency, this, std::placeholder::_1)
    );

    publishr_can = this->create_publisher<socketcan_interface_msg::msg::SocketcanIF>("can_tx", _qos);
    
    _pub_timer = this->create_wall_timer(
        std::chrono::milliseconds(interval_ms),
        [this] {_publisher_callback();}
    )
}

void CyberGearDriver::_publisher_callback(){
    if(mode == Mode::stop || mode == Mode::stay){
        vel = 0;
        tor = 0;
    }
}

void CyberGearDriver::_subscriber_callback_stop(const std_msgs::msg::Empty::SharedPtr msg){
    mode = Mode::stop;
    // RCLCPP_INFO(this->get_logger(), "停止");
}
void CyberGearDriver::_subscriber_callback_restart(const std_msgs::msg::Empty::SharedPtr msg){
    mode = Mode::stay;
    // RCLCPP_INFO(this->get_logger(), "再稼働");
}
void CyberGearDriver::_subscriber_callback_emergency(const socketcan_interface_msg::msg::SocketcanIF::SharedPtr msg){
    uint8_t _candata[8];
    for(int i=0; i<msg->candlc; i++) _candata[i] = msg->candata[i];

    if(_candata[6] and mode!=Mode::stop){
        mode = Mode::stop;
        // RCLCPP_INFO(this->get_logger(), "緊急停止!");
    }
}

void CyberGearDriver::send_rpm(const double angular_vel){
    auto msg_can = std::make_shared<socketcan_interface_msg::msg::SoketcanIF>();
    msg_can->canid = 0x00;
    msg_can->candlc = 0x7F;

    int32_t pos = static_cost<int32_t>((is_reverce_flag ? -1 : 1) * position * gear_ratio);
    int16_t vel = static_cost<int16_t>((is_reverce_flag ? -1 : 1) * velocity * gear_ratio);
    int16_t tor = static_cost<int16_t>(torque);

    uint8_t _candata[8];
    int_to_bytes(_candata, pos);
    int_to_bytes(_candata, vel);
    int_to_bytes(_candata, tor);

    for(int i=0; i<msg_can->candlc; i++) msg_can->candata[i]=_candata[i];
    piblisher_can->publish(*msg_can);
}

}