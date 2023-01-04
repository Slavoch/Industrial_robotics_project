# Industrial robotics final project
## Description
This project aims to manipulate robot via gestures.

- Program initialisation in simulator ([for realrobot](http://wiki.ros.org/fanuc/Tutorials/Running)): ```roslaunch fanuc_lrmate200id_moveit_config moveit_planning_execution.launch```
- Start controller node ```rosrun moveit_tutorials move_group_python_interface_tutorial.py```
- Start camera handler node ```rosrun cv_ind_robotics talker.py```
- Start gripper node ```rosrun cv_ind_robotics listener.py```

## Result:
- Robot takes the pan and waits for path data

![](/gifs/3.gif)

- Generation of path data and broadcast to the robot 
    * Using laptop camera get the on signal: touch index finger tip with middle finger tip
    * Draw the path (all points approx into polygon for simplicity for avoidance of the same point records)
    * Pass the polygon points to the robot in relative units using off signal: touch index finger tip with middle finger tip

![](/gifs/1.gif)
![](/gifs/2.gif)

- Robot draws and waits for another data

![](/gifs/4.gif)