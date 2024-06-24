#include "cybergear_driver/cybergear_driver_node.hpp"

using namespace utils;

namespace cybergear_driver{

CyberGearDriver::CyberGearDriver(const rclcpp::NodeOptions& options) : CyberGearDriver("", options) {}

CyberGearDriver::CyberGearDriver(const std::string& name_space, const rclcpp::NodeOptions& options)
: rclcpp::Node("cybergear_driver_node", name_space, options),
interval_ms(get_parameter("interval_ms").as_int()),
velocity_limit(get_parameter("velocity_limit").as_double()),
current_limit(get_parameter("current_limit").as_double()),
torque_limit(get_parameter("torque_limit").as_double()),
rotate_ratio(1.0 / get_parameter("reduction_ratio").as_double()),
position_kp(get_parameter("position_kp").as_double()),
velocity_kp(get_parameter("velocity_kp").as_double()),
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
        this->send_rpm(0.0, 0.0);
        return;

        vel = msg;
    }
    if(vel == nullptr) return;
    send_rpm(vel->z);
}

void CyberGearDriver::_subscriber_callback_stop(const std_msgs::msg::Empty::SharedPtr msg){
    mode = Mode::stop;
    RCLCPP_INFO(this->get_logger(), "停止");
}
void CyberGearDriver::_subscriber_callback_restart(const std_msgs::msg::Empty::SharedPtr msg){
    mode = Mode::stay;
    RCLCPP_INFO(this->get_logger(), "再稼働");
}
void CyberGearDriver::_subscriber_callback_emergency(const socketcan_interface_msg::msg::SocketcanIF::SharedPtr msg){
    uint8_t _candata[8];
    for(int i=0; i<msg->candlc; i++) _candata[i] = msg->candata[i];

    if(_candata[6] and mode!=Mode::stop){
        mode = Mode::stop;
        RCLCPP_INFO(this->get_logger(), "緊急停止!");
    }
}
void CyberGearDriver::_subscriber_callback_position(const std_msgs::msg::Empty::SharedPtr msg){
    mode = Mode::position;
    RCLCPP_INFO(this->get_logger(), "位置制御")
}
void CyberGearDriver::_subscriber_callback_velocity

void CyberGearDriver::send_rpm(const double angular_vel){
    const double vel = angular_vel;
    const double rpm = (is_reverse_ ? -1 : 1) * (vel*30 / d_pi) * rotate_ratio;

    auto msg_can = std::make_shared<socketcan_interface_msg::msg::SoketcanIF>();
    msg_can->canid = // 適切なcan idに変更
    msg_can->candlc = // ?

    uint8_t _candata[4];
    int_to_bytes(_candata, static_cast<int>(rpm));

    for(int i=0; i<msg_can->candlc; i++) msg_can->candata[i]=_candata[i];
    piblisher_can->publish(*msg_can);
}

}