# Sr. Design Project - Robotic Wheelchair

---
> Sr. Design project to track a wheelchair with a BeagleBone.

### Description
The goal of this project is to track the movements of a wheelchair through a closed environment using the 'dead reckoning' technique. This allows us to automate the actuation of various electronics as the user moves throughout the environment. For example, if the user moves from one room to another, we can turn off the lights in the room he/she is leaving and turn them on in the room he/she is entering.

### Implementation
The users movements are tracked using 'dead-reckoning'. This requires that we monitor all movements and apply them to a set of vectors (position and heading) at every increment. The movement of the wheels is monitored via a set of rotary encoders. These encoders translate small movements of the wheel into phase transitions of the encoder. We periodically read the encoder via GPIO and calculate movement vectors based on the difference in phase. 
