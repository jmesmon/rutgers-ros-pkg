/*
 * Ros.h
 *
 *  Created on: Oct 9, 2010
 *      Author: asher
 */

#ifndef ROS_H_
#define ROS_H_
#include "WProgram.h"
#include "ros_string.h"
#include "Msg.h"

#include "ros_packet.h"
#include "compiler.h"

typedef void (*ros_cb)(Msg* msg);


typedef uint8_t Publisher;

class Ros {
public:
	Ros(char * node_name);

	/* returns a uint8_t indicating the "publisher" */
	Publisher advertise(char* topic);

	void publish(Publisher pub, Msg* msg);

	void subscribe(char* name, ros_cb funct, Msg* msg);
	void spin(void);

	/* handles actually sending the data */
	void sendPkt(uint8_t pkt_type, uint8_t topic_id,
	              uint8_t* data, uint16_t length);

	/* depricated due to unintuitive argument ordering */
	__depricated
	void send(uint8_t* data, uint16_t length,
	          char packet_type, char topicID);

	~Ros();

	ROS::string name;
	uint8_t outBuffer[UINT8_MAX + 1];

private:
	void processPkt(struct packet_header *ptk);
	void getID(void);
	uint8_t getTopicTag(char * topic);

	/* These are all indexed by the same item */
	/* FIXME: determine count at compile time rather than fixing @ 10 */
	char* topic_list[10];
	ros_cb cb_list[10];
	Msg * msg_list[10];

	//variables for handling incoming packets
	void resetStateMachine(void);

	packet_header * header;
	uint8_t packet_data_left;
	uint8_t buffer[UINT8_MAX + 1];
	uint8_t buffer_index;

	enum packet_state {
		header_state,
		msg_data_state
	} com_state;
};

extern Ros ros;

void ros_init(void);


#endif /* ROS_H_ */
