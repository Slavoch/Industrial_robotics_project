#!/usr/bin/env python3.9
# Software License Agreement (BSD License)
#
# Copyright (c) 2008, Willow Garage, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of Willow Garage, Inc. nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Revision $Id$

## Simple talker demo that published std_msgs/Strings messages
## to the 'chatter' topic

import rospy
from std_msgs.msg import Float64MultiArray
import cv2
import mediapipe as mp
import numpy as np
mp_drawing = mp.solutions.drawing_utils
mp_hands = mp.solutions.hands

# For webcam input:



def talker():
    pub = rospy.Publisher('camera_data', Float64MultiArray, queue_size=1)
    rospy.init_node('talker', anonymous=True)
    now = rospy.get_rostime().secs
    mode = False
    x = 0
    y = 0
    cap = cv2.VideoCapture(0)
    pts = []
    poly = []
    MODE_DIST = 0.03
    while not rospy.is_shutdown():
        with mp_hands.Hands(
            min_detection_confidence=0.5,
            min_tracking_confidence=0.5) as hands:
            while cap.isOpened():
                success, image = cap.read()
                if not success:
                    print("Ignoring empty camera frame.")
                    continue

                image = cv2.cvtColor(cv2.flip(image, 1), cv2.COLOR_BGR2RGB)
                image_height,image_width =  image.shape[0:2]
                hh, ww = image.shape[0:2]
                start = np.array([0.15 * ww,0.15 * hh]).astype("int")
                end = np.array([0.85 * ww,0.85 * hh]).astype("int")
                cv2.rectangle(image,start,end,(255,0,0),2)
                # To improve performance, optionally mark the image as not writeable to
                # pass by reference.
                image.flags.writeable = False
                results = hands.process(image)
                image.flags.writeable = True

                image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
                
                if results.multi_hand_landmarks:
                    for hand_landmarks in results.multi_hand_landmarks:
                        mp_drawing.draw_landmarks(
                            image, hand_landmarks, mp_hands.HAND_CONNECTIONS)
                
                if results.multi_hand_landmarks is not None:
                    hand_landmarks = results.multi_hand_landmarks[-1]
                    thumb_x = hand_landmarks.landmark[mp_hands.HandLandmark.MIDDLE_FINGER_TIP].x # * image_width
                    thumb_y = hand_landmarks.landmark[mp_hands.HandLandmark.MIDDLE_FINGER_TIP].y #* image_height 
                    
                    x = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP].x #* image_width
                    y = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP].y #* image_height 
                    point1 = np.array([thumb_x,thumb_y])
                    point2 = np.array([x,y])

                    #rospy.loginfo(f"Push_data:{mode} {np.linalg.norm(point1 - point2)} {rospy.get_rostime().secs - now } ")
                    new_p = np.array([x*ww,y* hh]).astype("int")
                    if np.all(start < new_p) and np.all(new_p < end):
                        
                        if(np.linalg.norm(point1 - point2) <= MODE_DIST) and (abs(now - rospy.get_rostime().secs) > 1):
                            mode = not mode
                            now = rospy.get_rostime().secs
                        if mode:
                            pts.append([[int(x* image_width),int(y* image_height)]])
                            poly = cv2.approxPolyDP(np.array(pts), 0.02 * image_width, False)
                            image = cv2.polylines(image, [np.array(pts)],isClosed = False, color = (0,255,0), thickness = 2)
                            image = cv2.polylines(image, [poly], False, (0,0,255), 1)
                        else:
                            pts = []

                        #x,y = (x - 0.15) /70 * 100, (y - 0.15) /70 * 100
                        #print(x,y)

                cv2.imshow('MediaPipe Hands', image)
                if not mode and len(poly) > 1:
                    print("WHYYY")
                    print(poly.shape)
                    x_pts = (poly[:,0,0]/ww - 0.15) /70 * 100
                    y_pts = (poly[:,0,1]/hh - 0.15) /70 * 100
                    send_pts = np.hstack((x_pts,y_pts))
                    print(send_pts)
                    data_to_send = Float64MultiArray()
                    #data_to_send.data = [float(mode),x,y]
                    data_to_send.data = send_pts
                    pub.publish(data_to_send)
                    poly = np.array([])
                if cv2.waitKey(5) & 0xFF == 27:
                    break

        cap.release()
        cv2.destroyAllWindows()
	

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass
