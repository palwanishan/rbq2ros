#include <QCoreApplication>
#include <QThread>
#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <sensor_msgs/Imu.h>
#include "IndexNotation.h"
#include "client.h"

pRBCORE_SHM sharedData;

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  QThread::currentThread()->setObjectName("Main Thread");
  QThread thread;
  thread.setObjectName("Client Thread");
  Client* client = new Client();
  client->moveToThread(&thread);
  QObject::connect(&thread, &QThread::started,client,&Client::run);
  client->setHost("192.168.0.10"); client->setPort(8000);

	sharedData = (pRBCORE_SHM)malloc(sizeof(RBCORE_SHM));

  thread.start();

	ros::init(argc, argv, "rbq2ros");
	ros::Time::init();
	ros::Rate loop_rate(1);
	ros::NodeHandle node_handle_;
	ros::Publisher rbq3_joint_states_pub_;
	ros::Publisher rbq3_imu_pub_;
	rbq3_joint_states_pub_ = node_handle_.advertise<sensor_msgs::JointState>("rbq3/joint_states", 100);
	rbq3_imu_pub_ = node_handle_.advertise<sensor_msgs::Imu>("rbq3/imu", 100);
	std::vector<std::string> jnt_names{"HRR", "HRP", "HRK", "HLR", "HLP", "HLK", "FRR", "FRP", "FRK", "FLR", "FLP", "FLK"};

	while(ros::ok())
	{
		sensor_msgs::JointState msg;
		msg.header.stamp = ros::Time::now();
		for(uint8_t i = 0; i < 12; i++){
			msg.name.push_back(jnt_names.at(i));
			msg.position.push_back(sharedData->ROBOT_DATA.State.joint[i].pos);
			msg.velocity.push_back(sharedData->ROBOT_DATA.State.joint[i].vel);
			msg.effort.push_back(sharedData->ROBOT_DATA.State.joint[i].torque);
		}rbq3_joint_states_pub_.publish(msg);
		
    sensor_msgs::Imu imu_msg;
		imu_msg.header.stamp = ros::Time::now();
    imu_msg.orientation.x = sharedData->ROBOT_DATA.Sensor.imu.quat.x();
    imu_msg.orientation.y = sharedData->ROBOT_DATA.Sensor.imu.quat.y();
    imu_msg.orientation.z = sharedData->ROBOT_DATA.Sensor.imu.quat.z();
    imu_msg.orientation.w = sharedData->ROBOT_DATA.Sensor.imu.quat.w();
    rbq3_imu_pub_.publish(imu_msg);


		if(sharedData->LanComm_Status)
		std::cout << sharedData->ROBOT_DATA.Sensor.imu.rpy[0] << "\t"
							<< sharedData->ROBOT_DATA.Sensor.imu.rpy[1] << "\t"
							<< sharedData->ROBOT_DATA.Sensor.imu.rpy[2] << std::endl;

		ros::spinOnce();
		loop_rate.sleep();
	}
  return a.exec();
}