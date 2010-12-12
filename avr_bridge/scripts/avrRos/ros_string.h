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
	string(size_t len);
	string(char const *str);
	virtual ~string(void);

	void setString(char const *str);
	size_t bytes(void);
	size_t serialize(uint8_t *buffer);
	size_t deserialize(uint8_t const* buffer);
	char operator[](int i){ return data[i];};
	char *operator&(void) { return data;};
	void setMaxLength(uint16_t maxLength);

private:
	char  *_buffer;
	size_t _buffer_len;
	size_t _string_len;
};

}
#endif /* ROS_STRING_H_ */
