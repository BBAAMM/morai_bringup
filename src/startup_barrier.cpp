#pragma once
#include "ros/time.h"
#include <ros/ros.h>
#include <ros/master.h>
#include <vector>
#include <string>

namespace morai::bringup {
    static bool topicsReady(const std::vector<std::string>& required_topics){
        ros::master::V_TopicInfo infos;
        if(!ros::master::getTopics(infos)) return false;
        for(const auto& topic : required_topics){
            bool ok = false;
            for(const auto& info : infos) if(info.name == topic){ ok = true; break; }
            if(!ok) return false;
        }
        return true;
    }
}

int main(int argc, char** argv){
    ros::init(argc, argv, "startup_barrier");
    ros::NodeHandle pnh("~");
    std::vector<std::string> required{"/ExampleTopic"};
    pnh.getParam("required_topics", required);
    const double timeout = pnh.param("timeout_sec", 20.0);

    ros::Time end = ros::Time::now() + ros::Duration(timeout);
    ros::Rate rate(1.0);
    while(ros::ok()&&(ros::Time::now() < end)){
        if(morai::bringup::topicsReady(required)){
            ROS_INFO("Startup barrier passed.");
            return 0;
        }
        ROS_WARN_THROTTLE(2.0, "Waiting for required topics...");
        rate.sleep();
    }
    ROS_ERROR("Startup barrier timeout. Missing required topics.");
    return 1;
}