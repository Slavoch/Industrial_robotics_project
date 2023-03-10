//
// Created by artemyakimchuk on 20/11/2019.
//


#include "ros/ros.h"
#include <geometry_msgs/Pose.h>
#include "tf/transform_datatypes.h"
#include <tf/transform_listener.h>
#include "std_msgs/String.h"
#include <visualization_msgs/Marker.h>
#include <moveit/robot_model/robot_model.h>
#include <moveit/robot_state/robot_state.h>
#include <math.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <moveit_msgs/CollisionObject.h>
#include <moveit_visual_tools/moveit_visual_tools.h>
#include <moveit/move_group/capability_names.h>
#include <moveit/trajectory_processing/iterative_time_parameterization.h>
#include <moveit/trajectory_processing/time_optimal_trajectory_generation.h>
#include <geometric_shapes/mesh_operations.h>
#include <geometric_shapes/shape_operations.h>

#define DEG_TO_RAD (M_PI/180.0)


std::vector<geometry_msgs::Pose> createWaypoints(geometry_msgs::Pose primitive, double dim_a, double dim_b) {
    tf::Transform rot_tcp;
    double radius = 0.005;
    double offset = 0.05;
    double delta = 0.001;
    double n = ceil(dim_b / (2 * ((2 * radius) - delta)));
    double r, p, y;
    tf::poseMsgToTF(primitive, rot_tcp);
    //rot_tcp.getBasis().getRPY(p,r,y); // roll and pitch are different cause base_link do not match world
    rot_tcp.getBasis().getRPY(r, p, y); // roll and pitch are different cause base_link do not match world
    //y-= M_PI/2 ;
    //y-= 0 ;

    //p +=M_PI/2;
    //r +=M_PI/2;

    std::vector<geometry_msgs::Pose> waypoints;
    tf::Transform tf_of_object, tf_current;
    geometry_msgs::Pose point;
    tf::poseMsgToTF(primitive, tf_of_object);

    tf_of_object.getBasis().setRPY(r, p, y);

//    tf_current.getOrigin().setX(- dim_b/2-(2*radius));
//    tf_current.getOrigin().setY(+ dim_a/2+(radius));
    tf_current.getOrigin().setX(-dim_b / 2);
    tf_current.getOrigin().setY(+dim_a / 2);
    tf_current.getOrigin().setZ(-offset);
    tf_current.getBasis().setRPY(0, 0, 0);

    tf_of_object *= tf_current;

    tf::poseTFToMsg(tf_of_object, point);
    waypoints.push_back(point);

    for (int i = 0; i < n; ++i) {
        //tf_of_object.getOrigin().setZ((tf_of_object.getOrigin().z() - dim_a - 2*(2*radius))) ;

        tf_current.getOrigin().setZero();
        tf_current.getOrigin().setX(-(-dim_b - 2 * (2 * radius)));
        tf_of_object *= tf_current;
        tf::poseTFToMsg(tf_of_object, point);
        waypoints.push_back(point);

        //tf_of_object.getOrigin().setX((tf_of_object.getOrigin().x() + (2*radius)-delta)) ;
        tf_current.getOrigin().setZero();
        tf_current.getOrigin().setY(-((2 * radius) - delta));
        tf_of_object *= tf_current;
        tf::poseTFToMsg(tf_of_object, point);
        waypoints.push_back(point);

        //tf_of_object.getOrigin().setZ((tf_of_object.getOrigin().z() + dim_a + 2*(2*radius))) ;
        tf_current.getOrigin().setZero();
        tf_current.getOrigin().setX(-(+dim_b + 2 * (2 * radius)));
        tf_of_object *= tf_current;
        tf::poseTFToMsg(tf_of_object, point);
        waypoints.push_back(point);

        //tf_of_object.getOrigin().setX((tf_of_object.getOrigin().x() + (2*radius)-delta)) ;
        tf_current.getOrigin().setZero();
        tf_current.getOrigin().setY(-(+(2 * radius) - delta));
        tf_of_object *= tf_current;
        tf::poseTFToMsg(tf_of_object, point);
        waypoints.push_back(point);

    }
    return waypoints;

}

void addPrimitiveBox(moveit::planning_interface::PlanningSceneInterface &planning_scene_interface,
                     moveit::planning_interface::MoveGroupInterface &group,
                     std::string box_id,
                     tf::Vector3 box_size,
                     tf::Transform box_tf
) {
    //Add primitive object for process
    moveit_msgs::CollisionObject collision_object;
    collision_object.header.frame_id = group.getPlanningFrame();
    collision_object.id = box_id;
    shape_msgs::SolidPrimitive primitive;
    primitive.type = primitive.BOX;
    primitive.dimensions.resize(3);
//    primitive.dimensions[0] = 0.3;
//    primitive.dimensions[1] = 0.15;
//    primitive.dimensions[2] = 0.3;
    primitive.dimensions[0] = box_size.getX();
    primitive.dimensions[1] = box_size.getY();
    primitive.dimensions[2] = box_size.getZ();

    geometry_msgs::Pose box_pose;
    tf::poseTFToMsg(box_tf, box_pose);


    collision_object.primitives.push_back(primitive);
    collision_object.primitive_poses.push_back(box_pose);
    collision_object.operation = collision_object.ADD;
    std::vector<moveit_msgs::CollisionObject> collision_objects;
    collision_objects.push_back(collision_object);
    //Add collision object to the world
    ROS_INFO_NAMED("tutorial", "Add an object into the world");
    planning_scene_interface.addCollisionObjects(collision_objects);

}

std::vector<geometry_msgs::Pose> Interpolate(geometry_msgs::Pose n, geometry_msgs::Pose m) {
    //Define the biggest shortcut of xyz.
    std::vector<geometry_msgs::Pose> InterWayPoints;
    InterWayPoints.clear();
    //step size is 1mm;
    float delta = 0.005;
    float d_x = m.position.x - n.position.x;
    float d_y = m.position.y - n.position.y;
    float d_z = m.position.z - n.position.z;
    float d_max = std::max({abs(d_x), abs(d_y), abs(d_z)});
    int steps = d_max / delta; //define number of steps in the biggest shortcut
    geometry_msgs::Pose current_pose = n;

    for (int i = 0; i < steps; ++i) {
        current_pose.position.x += d_x / steps;
        current_pose.position.y += d_y / steps;
        current_pose.position.z += d_z / steps;
        InterWayPoints.push_back(current_pose);
    }
    return InterWayPoints;
}

std::vector<geometry_msgs::Pose> InterWayPoints(std::vector<geometry_msgs::Pose> waypoints) {
    std::vector<geometry_msgs::Pose> inter_waypoints, new_points;
    inter_waypoints.clear();
    for (int n = 0; n < waypoints.size() - 1; ++n) {
        //Here we attach interpolated trajectory to existing position vector by insert function
        new_points = Interpolate(waypoints[n], waypoints[n + 1]);
        inter_waypoints.insert(std::end(inter_waypoints), std::begin(new_points), std::end(new_points));
        new_points.clear();
    }
    return inter_waypoints;
}

int main(int argc, char **argv) {

    ROS_INFO_NAMED("tutorial", "trajectory execution node has started");
    ros::init(argc, argv, "ur5_control");
    ros::NodeHandle n("~");
    if (n.ok()) {
        ROS_INFO("ok status was received from n.ok() check");
    }


    ros::AsyncSpinner spinner(2);
    spinner.start();

    //Initialize MoveGroup
    moveit::planning_interface::MoveGroupInterface group("manipulator");
    group.setPlanningTime(20.5);
    group.setPlannerId("RRTConnectkConfigDefault");
    group.setEndEffectorLink("tcp");
    const ::robot_state::JointModelGroup *joint_model_group = group.getCurrentState()->getJointModelGroup(
            "manipulator");
    moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

    //init rviz topic for markers
    namespace rvt = rviz_visual_tools;
    moveit_visual_tools::MoveItVisualTools visual_tools("base_link");
    visual_tools.deleteAllMarkers();
    visual_tools.loadRemoteControl();

    ros::Publisher marker_pub = n.advertise<visualization_msgs::MarkerArray>("/markers", 1, false);
	ros::Rate r(30);
	float f = 0.0;

	Eigen::Isometry3d text_pose = Eigen::Isometry3d::Identity();
    text_pose.translation().z() = 0.5;
    visual_tools.publishText(text_pose, "Linear Motion Demo", rvt::WHITE, rvt::XLARGE);
    visual_tools.trigger();


    tf::StampedTransform object_transform;
    tf::TransformListener listener_for_tf;
    tf::Vector3 vec(0, 0, 0);
    object_transform.setOrigin(vec);

    visual_tools.trigger();

    ROS_INFO_NAMED("tutorial", "Planning frame: %s", group.getPlanningFrame().c_str());
    ROS_INFO_NAMED("tutorial", "End effector link: %s", group.getEndEffectorLink().c_str());
    ROS_INFO_NAMED("tutorial", "Available Planning Groups:");

    std::copy(group.getJointModelGroupNames().begin(), group.getJointModelGroupNames().end(),
              std::ostream_iterator<std::string>(std::cout, ", "));

    while(n.ok()) {

		group.setStartStateToCurrentState();
		group.setMaxVelocityScalingFactor(0.9);
		moveit::planning_interface::MoveGroupInterface::Plan my_plan;


		bool success = (group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
		ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");
		moveit::core::RobotStatePtr current_state = group.getCurrentState();
		std::vector<double> joint_group_positions;
		current_state->copyJointGroupPositions(joint_model_group, joint_group_positions);

//		box_tf.setRotation(rot_point1);
//		box_tf.setOrigin(xyz_point1);




//		box_tf.setRotation(rot_point2);
//		box_tf.setOrigin(xyz_point2);
//		tf::poseTFToMsg(box_tf, box_pose2);
//		visual_tools.publishAxisLabeled(box_pose2, "object", rvt::SMALL);
		 visual_tools.trigger();
		 visual_tools.prompt("Second step. Press NEXT to plan to CAMERA position");
//		 joint_group_positions[0] = 5 * DEG_TO_RAD;
//		 joint_group_positions[1] = 90 * DEG_TO_RAD;
//		 joint_group_positions[2] = 5 * DEG_TO_RAD;
//		 joint_group_positions[3] = 5 * DEG_TO_RAD;
//		 joint_group_positions[4] = 90 * DEG_TO_RAD;
//		 joint_group_positions[5] = 5 * DEG_TO_RAD;
        geometry_msgs::Pose camera_pose;
        tf::Transform camera_tf;
        tf::Vector3 xyz_point1;
        tf::Quaternion rot_point1;
        xyz_point1.setValue(0.2 ,0.1,0.1);
        rot_point1.setValue(1,1,0,0);
        camera_tf.setOrigin(xyz_point1);
        camera_tf.setRotation(rot_point1);
        tf::poseTFToMsg(camera_tf, camera_pose);
        group.setPoseTarget(camera_pose);
        visual_tools.publishAxisLabeled(camera_pose, "Position 1", rvt::SMALL);
        visual_tools.trigger();
        //group.setJointValueTarget(joint_group_positions);
        visual_tools.prompt("Press NEXT to MOVE to Camera2 position");
        group.move();
        visual_tools.prompt("Press NEXT to PLAN to STICK position");
        geometry_msgs::Pose new_camera_pose;
        tf::Transform new_camera_tf;
        tf::Vector3 xyz_point2;
        tf::Quaternion rot_point2;
        xyz_point2.setValue(0.2 ,0.1,0.1);
        rot_point2.setValue(1,1,0,0);
        new_camera_tf.setOrigin(xyz_point2);
        new_camera_tf.setRotation(rot_point2);
        tf::poseTFToMsg(new_camera_tf, new_camera_pose);
//        group.setPoseTarget(new_camera_pose);

        std::vector<geometry_msgs::Pose> waypoints;
        waypoints.push_back(new_camera_pose);
        xyz_point2.setValue(0.3 ,0.21,0.1);
        rot_point2.setValue(1,1,0,0);
        new_camera_tf.setOrigin(xyz_point2);
        new_camera_tf.setRotation(rot_point2);
        tf::poseTFToMsg(new_camera_tf, new_camera_pose);
        waypoints.push_back(new_camera_pose);

        moveit_msgs::RobotTrajectory trajectory;
        const double jump_threshold = 0.0;
        const double eef_step = 0.01;
        double fraction = group.computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);
        ROS_INFO_NAMED("tutorial", "Visualizing plan 4 (Cartesian path) (%.2f%% acheived)", fraction * 100.0);
        visual_tools.prompt("Press NEXT to MOVE to STICK position");
        group.execute(trajectory);


        success = (group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
        ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");

//		 group.setJointValueTarget(joint_group_positions);
		 success = (group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
		 ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");
		 visual_tools.prompt("Press NEXT to MOVE to CAMERA position");
		 group.move();

         /*
        visual_tools.prompt("Press NEXT to get Object Transform");
        if (n.ok()) {
            ROS_INFO("Waiting for transform....");
        }
        if (listener_for_tf.waitForTransform("base_link", "object_pose", ros::Time(), ros::Duration(10.0))) {
            listener_for_tf.lookupTransform("base_link", "object_pose", ros::Time(0), object_transform);
            ros::spinOnce();
        }

        tf::Transform box_tf;
        geometry_msgs::Pose box_pose1, box_pose2, stick_location;
//        tf::Vector3 xyz_point1, xyz_point2;
//        tf::Quaternion rot_point1, rot_point2;
//        xyz_point1.setValue(0.4,0.0,0.3);
//        rot_point1.setRPY(90 * DEG_TO_RAD, 0, 0);
//
//        xyz_point2.setValue(0.45,0.0,0.4);
//        rot_point2.setRPY(90 * DEG_TO_RAD, 0, 0);
        box_tf.setRotation(object_transform.getRotation());
        box_tf.setOrigin(object_transform.getOrigin());
        tf::poseTFToMsg(box_tf, stick_location);
        visual_tools.publishAxisLabeled(stick_location, "STICK_tf", rvt::SMALL);
        visual_tools.trigger();
		visual_tools.prompt("Press NEXT to calculate PRE stick location");

		geometry_msgs::Pose current_pose;
		geometry_msgs::Pose pre_stick_location;
        pre_stick_location = stick_location;
        pre_stick_location.position.z += 0.1;

        stick_location.position.z -+0.055;

/////////////////////////////////////////////////////////////////////////////////////////////
/*
		// Cartesian Paths
		//Create basic parameters for traj
		// std::vector<geometry_msgs::Pose> waypoints;
		// std::vector<geometry_msgs::Pose> inter_waypoints;
		// moveit_msgs::RobotTrajectory trajectory;
		// const double jump_threshold = 0.0;
		// const double eef_step = 0.05;
		// robot_trajectory::RobotTrajectory rt(group.getRobotModel(), group.getName());
		// trajectory_processing::TimeOptimalTrajectoryGeneration time_param;

		//    //Add primitive object for process
		// moveit_msgs::CollisionObject collision_object;
		// collision_object.header.frame_id = group.getPlanningFrame();
		// shape_msgs::SolidPrimitive primitive;
		// primitive.type = primitive.BOX;

		// tf::Vector3 box_size;
		// box_size.setX(0.105);
		// box_size.setY(0.115);
		// box_size.setZ(0.05);
		// tf::Transform box_tf;
		// ///////////////////////////////////////////////////////////////ADD TF HERE

		// box_tf.setRotation(object_transform.getRotation());
		// box_tf.setOrigin(object_transform.getOrigin());

		// ros::NodeHandle nh;
		// std::string stl_path;
		// if (nh.getParam("/trajectory_node/stl_path", stl_path)) {
		// 	ROS_INFO_NAMED("cv node", "Path to stl file: %s", stl_path.c_str());
		// }
		// Eigen::Vector3d vectorScale(1, 1, 1);
		// std::string stl_path_with_prefix = "file://" + stl_path;
		// shapes::Mesh *meshObject = shapes::createMeshFromResource(stl_path_with_prefix, vectorScale);
		// shape_msgs::Mesh mesh;
		// shapes::ShapeMsg meshMessage;
		// shapes::constructMsgFromShape(meshObject, meshMessage);
		// // Now, let's add the collision object into the world
		// ROS_INFO_NAMED("DEMO", "OBJECT IN THE WORLD");
		// mesh = boost::get<shape_msgs::Mesh>(meshMessage);
		// collision_object.meshes.push_back(mesh);
		// geometry_msgs::Pose box_pose;
		// tf::poseTFToMsg(box_tf, box_pose);
		// collision_object.mesh_poses.push_back(box_pose);

		// std::vector<moveit_msgs::CollisionObject> collision_objects;
		// collision_objects.push_back(collision_object);
		// planning_scene_interface.addCollisionObjects(collision_objects);

		// visual_tools.publishAxisLabeled(box_pose, "object", rvt::SMALL);
		//Add collision object to the world

		// visual_tools.publishText(text_pose, "Add object", rvt::WHITE, rvt::XLARGE);
*/
		// Cartesian movespublishAxisLabeled
		//visual_tools.trigger();
		//visual_tools.prompt("Press NEXT to plan to pre stick location");

/*		// std::vector<geometry_msgs::Pose> waypoints;

		// geometry_msgs::Pose src_pose;
		// src_pose.position.x = 0.5;
		// src_pose.position.y = 0.0;
		// src_pose.position.z = 0.3;
		// src_pose.orientation.x = 0;
		// src_pose.orientation.y = 0;
		// src_pose.orientation.z = 0;
		// src_pose.orientation.w = 0;

		// waypoints.push_back(box_pose);

		// geometry_msgs::Pose trgt_pose = src_pose;
		// trgt_pose.position.x = 0.1;
		// trgt_pose.position.y = 0.1;
		// trgt_pose.position.z = 0.1;
		// waypoints.push_back(trgt_pose);

		// visual_tools.deleteAllMarkers();
		// visual_tools.publishText(text_pose, "Cartesian Path", rvt::WHITE, rvt::XLARGE);
		// visual_tools.publishPath(waypoints, rvt::LIME_GREEN, rvt::SMALL);
		// for (std::size_t i = 0; i < waypoints.size(); ++i)
		//   visual_tools.publishAxisLabeled(waypoints[i], "pt" + std::to_string(i), rvt::SMALL);
		// visual_tools.trigger();
		// visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to continue the demo");


		// moveit_msgs::RobotTrajectory trajectory;
		// const double jump_threshold = 0.0;
		// const double eef_step = 0.001;
		// double fraction = group.computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);
		// ROS_INFO_NAMED("tutorial", "Visualizing plan 4 (Cartesian path) (%.2f%% acheived)", fraction * 100.0);
		// visual_tools.prompt("Press NEXT to MOVE");
		// group.move();

		// visual_tools.trigger();
		// visual_tools.prompt("First step. Press NEXT to plan to another position");

		// const float small_deviation = 10 * DEG_TO_RAD;
		// // joint_group_positions[0] = 5 * DEG_TO_RAD;
		// // joint_group_positions[1] = 5 * DEG_TO_RAD;
		// // joint_group_positions[2] = 5 * DEG_TO_RAD;
		// // joint_group_positions[3] = 0 * DEG_TO_RAD;
		// // joint_group_positions[4] = -90 * DEG_TO_RAD;
		// // joint_group_positions[5] = 40 * DEG_TO_RAD;
		// const float eps = 5;

		// joint_group_positions[0] += eps * DEG_TO_RAD;
		// joint_group_positions[1] += eps * DEG_TO_RAD;
		// joint_group_positions[2] += eps * DEG_TO_RAD;
		// joint_group_positions[3] += eps * DEG_TO_RAD;
		// joint_group_positions[4] += eps * DEG_TO_RAD;
		// joint_group_positions[5] += eps * DEG_TO_RAD;

*/
			// group.setJointValueTarget(joint_group_positions);
/*
//		group.setPoseTarget(pre_stick_location);
//        success = (group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
//        ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");
//
//		visual_tools.prompt("Press NEXT to MOVE to PRE_stick position");
//		group.move();
//
//
//		//visual_tools.trigger();
//		visual_tools.prompt("Press NEXT to plan to STICK position");

//		std::vector<geometry_msgs::Pose> waypoints;
//		waypoints.push_back(stick_location);
//
//
//		moveit_msgs::RobotTrajectory trajectory;
//		const double jump_threshold = 0.0;
//		const double eef_step = 0.003;
//		double fraction = group.computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);
//		ROS_INFO_NAMED("tutorial", "Visualizing plan 4 (Cartesian path) (%.2f%% acheived)", fraction * 100.0);
//		visual_tools.prompt("Press NEXT to MOVE to STICK position");
//		group.execute(trajectory);

//        visual_tools.prompt("Press NEXT to GRAB the STICK");
//        // EXECUTION OF THE GRIPPER
//        int result = system("python /home/avya/Documents/thermalspray_cell/catkin_ws/src/fanuc_experimental/fanuc_control/src/GripperManipulator.py  --statusint 1  --COM /dev/ttyACM0");
//        std::cout << result;
//
//        visual_tools.prompt("Press NEXT to PLAN to MIDDLE position");
//        joint_group_positions[0] = 0 * DEG_TO_RAD;
//        joint_group_positions[1] = 0 * DEG_TO_RAD;
//        joint_group_positions[2] = 0 * DEG_TO_RAD;
//        joint_group_positions[3] = 0 * DEG_TO_RAD;
//        joint_group_positions[4] = -90 * DEG_TO_RAD;
//        joint_group_positions[5] = 135 * DEG_TO_RAD;
//
//
//        group.setJointValueTarget(joint_group_positions);
//        success = (group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
//        ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");
//        visual_tools.prompt("Press NEXT to MOVE to MIDDLE position");
//        group.move();
//
//        visual_tools.prompt("Press NEXT to PLAN to PLACE position");
//        joint_group_positions[0] = 10 * DEG_TO_RAD;
//        joint_group_positions[1] = 0 * DEG_TO_RAD;
//        joint_group_positions[2] = 0 * DEG_TO_RAD;
//        joint_group_positions[3] = 0 * DEG_TO_RAD;
//        joint_group_positions[4] = -90 * DEG_TO_RAD;
//        joint_group_positions[5] = 135 * DEG_TO_RAD;
//        group.setJointValueTarget(joint_group_positions);
//        success = (group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
//        ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");
//        visual_tools.prompt("Press NEXT to MOVE to PLACE position");
//        group.move();
//
//        visual_tools.prompt("Press NEXT to RELEASE the STICK");
//        // EXECUTION OF THE GRIPPER - OFF
//        result = system("python /home/avya/Documents/thermalspray_cell/catkin_ws/src/fanuc_experimental/fanuc_control/src/GripperManipulator.py  --statusint 0  --COM /dev/ttyACM0");
//        std::cout << result;
*/
        // // # 2
        /*
		// visual_tools.trigger();
		// visual_tools.prompt("Second step. Press NEXT to plan to another position");

		// // joint_group_positions[0] = 5 * DEG_TO_RAD;
		// // joint_group_positions[1] = 5 * DEG_TO_RAD;
		// // joint_group_positions[2] = 5 * DEG_TO_RAD;
		// // joint_group_positions[3] = 0 * DEG_TO_RAD;
		// // joint_group_positions[4] = -90 * DEG_TO_RAD;
		// // joint_group_positions[5] = 40 * DEG_TO_RAD;

		// joint_group_positions[0] -= eps * DEG_TO_RAD;
		// joint_group_positions[1] -= eps * DEG_TO_RAD;
		// joint_group_positions[2] -= eps * DEG_TO_RAD;
		// joint_group_positions[3] -= eps * DEG_TO_RAD;
		// joint_group_positions[4] -= eps * DEG_TO_RAD;
		// joint_group_positions[5] -= eps * DEG_TO_RAD;


		// group.setJointValueTarget(joint_group_positions);
		// success = (group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
		// ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");
		// visual_tools.prompt("Press NEXT to MOVE to HOME position");
		// group.move();
		
		// visual_tools.publishPath(inter_waypoints, rvt::LIME_GREEN, rvt::SMALL);
        */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Wait for MoveGroup to recieve and process the attached collision object message */
/*
		// ROS_INFO_NAMED("DEMO", "Remove everything from the world");
		// std::vector<std::string> object_ids;
		// object_ids.push_back(collision_object.id);
		// planning_scene_interface.removeCollisionObjects(object_ids);

		// visual_tools.publishText(text_pose, "Object removed", rvt::WHITE, rvt::XLARGE);
		// visual_tools.deleteAllMarkers();
		// visual_tools.trigger();
        */
	}
    // ros::shutdown();
    // return 0;
}
