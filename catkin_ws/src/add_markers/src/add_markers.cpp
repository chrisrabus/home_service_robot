#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <std_msgs/Int32.h>  //included for deriving state of robot


class Listener
{
  int stateRobot;
public:
  void RobState(const std_msgs::Int32::ConstPtr& msg);
  int getState(void);
  //int stateRobot;
  Listener ();
};

Listener::Listener(void)
{
  stateRobot = 0;
}

void Listener::RobState(const std_msgs::Int32::ConstPtr& msg)
{
  stateRobot = msg->data;
  ROS_INFO("Stated changed to: [%i]", stateRobot);
}

int Listener::getState(void)
{
  return stateRobot;
}

int main( int argc, char** argv )
{
  ros::init(argc, argv, "add_markers");
  ros::NodeHandle n;
  ros::Rate r(1000);
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);
  
  Listener listener;
  ros::Subscriber sub_state = n.subscribe("StateRobot", 3, &Listener::RobState, &listener);
  
  // Set our initial shape type to be a cube
  uint32_t shape = visualization_msgs::Marker::CUBE;

  visualization_msgs::Marker marker;
  // Set the frame ID and timestamp.  See the TF tutorials for information on these.
  marker.header.frame_id = "map";
  marker.header.stamp = ros::Time::now();

  // Set the namespace and id for this marker.  This serves to create a unique ID
  // Any marker sent with the same namespace and id will overwrite the old one
  marker.ns = "cube";
  marker.id = 0;

  // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
  marker.type = shape;

  // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
  marker.action = visualization_msgs::Marker::ADD;

  // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
  //Pick up Zone
  marker.pose.position.x = 4.0;
  marker.pose.position.y = 6.1;  // little offset, so that robot does not complete hide the object
  marker.pose.position.z = 0;
  marker.pose.orientation.x = 0.0;
  marker.pose.orientation.y = 0.0;
  marker.pose.orientation.z = 0.0;
  marker.pose.orientation.w = 1.0;

  // Set the scale of the marker -- 1x1x1 here means 1m on a side
  marker.scale.x = 0.35;
  marker.scale.y = 0.35;
  marker.scale.z = 0.35;

  // Set the color -- be sure to set alpha to something non-zero!
  marker.color.r = 0.0f;
  marker.color.g = 1.0f;
  marker.color.b = 0.0f;
  marker.color.a = 1.0;

  marker.lifetime = ros::Duration();
  while (marker_pub.getNumSubscribers() < 1)
  {
    if (!ros::ok())
    {
      return 0;
    }
    ROS_WARN_ONCE("Please create a subscriber to the marker");
    sleep(1);
  }
  
  while (ros::ok())
  {
   	if (listener.getState() == 0)
    {  
      // Robot is at starting point
    }
    else if (listener.getState() == 1)
    {
      // Robot is moving to pick up zone
    }
    else if (listener.getState() == 2)
    {
      //robot reached pick up zone, hiding marker
      marker.color.a = 0.0;
    }
    else if (listener.getState() == 3)
    {
      //robot is moving to drop off zone, change position for marker
      marker.pose.position.x = -4.0;
    }
    else if (listener.getState() == 4)
    {
      //robot reached drop off zone, show marker
      marker.color.a = 1.0;
    }
    else
    {
    }    
    marker_pub.publish(marker);
    ros::spinOnce();
  }
  return 0;
}