#!/usr/bin/env python
import time
import rospy
from std_msgs.msg import Float32
from std_msgs.msg import Int32

def callback(Button):
    #Funcion developed after the callback
    motor(Button)

def motor(Button):
    #create a node called Print sending an integer
    pub=rospy.Publisher('Print', Int32, queue_size=10) #rospy.Publisher(Topic name, Type, queue_size 
    #rospy.init_node('motor',anonymous=True) #rospy.init_node(Node name,anonymous=True)
    rate=rospy.Rate(57600) #10Hz
    if Button.data > 620:
        #when the flame sensor in the middle is above 620 the code publish a 1 in order to create a movement in the arduino file
        pub.publish(1)  
        rospy.loginfo("Open hose")       
    elif Button.data < 620:
        #whem the flame sensor in the middle is under 620 the code publish a 0 so the servomotor keep close
        pub.publish(0)
        rospy.loginfo("Close hose")  
    rate.sleep()

    
def listener():
    # Node named resistencia
    rospy.init_node('resistencia', anonymous=True) #anonymus let us have many node named the same way running simultaneously
    #Create a topic called Button where the python code is subcribed, receiving a integer as input
    rospy.Subscriber('Button', Int32,callback) #Callback name of the function you need to call when you get data
    rospy.spin() #keeps the program running until the node is stopped


if __name__=='__main__':
    listener()
