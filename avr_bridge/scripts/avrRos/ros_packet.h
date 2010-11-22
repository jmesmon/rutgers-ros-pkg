#ifndef ROS_PACKET_H_
#define ROS_PACKET_H_

#include "compiler.h"

/* packet structure and field definitions for avr<-> pc communication */

struct packet_header {
	uint8_t packet_type;
#define PT_TOPIC 0
#define PT_SERVICE 1
#define PT_GETID 255

	uint8_t topic_tag;
	uint16_t msg_length;
	uint8_t data[];
} __packed;


#endif
