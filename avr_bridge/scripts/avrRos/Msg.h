/*
 * Msg.h
 *
 *  Created on: Oct 9, 2010
 *      Author: asher
 */

#ifndef MSG_H_
#define MSG_H_

#include <stdint.h>

class Msg {
public:
	Msg();
	Msg(uint8_t* data);
	virtual uint16_t serialize(uint8_t * outbuffer)=0;
	virtual uint16_t deserialize(uint8_t * data)=0;
	virtual uint16_t bytes()=0;
	 ~Msg();
private:
	char * _buffer;
	int _length; //length of byte stream;

	/* returns the length of the serialized data.
	 * XXX: UNSAFE: outbuffer cannot be not length checked
	 */
	virtual uint16_t serialize(uint8_t * outbuffer) = 0;

	/* ???
	 * How is length of data indicated?
	 */
	virtual uint16_t deserialize(uint8_t * data) = 0;


	virtual uint16_t bytes() = 0;
	virtual ~Msg() = 0;
};

#endif /* MSG_H_ */
