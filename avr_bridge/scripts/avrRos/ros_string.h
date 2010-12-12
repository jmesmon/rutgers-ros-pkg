/*
 * string.h
 *
 *  Created on: Nov 1, 2010
 *      Author: asher
 */

#ifndef ROS_STRING_H_
#define ROS_STRING_H_
#include <stdint.h>

//this is only meant for rosdatatypes

namespace ROS {

class string {
public:
	string(void);
	string(uint16_t maxLength);
	string(char *str);

	void setString(char *str);
	uint16_t bytes(void);
	uint16_t serialize(uint8_t *buffer);
	uint16_t deserialize(uint8_t *buffer);
	char operator[](int i){ return data[i];};
	char *operator&(void) { return data;};
	void setMaxLength(uint16_t maxLength);

private:
	char *data;
	uint16_t maxlength;
};

}
#endif /* ROS_STRING_H_ */
