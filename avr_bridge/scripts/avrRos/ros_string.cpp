/*
 * ros_string.cpp
 *
 *  Created on: Nov 1, 2010
 *      Author: asher
 */

#include "ros_string.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace ROS;


string::string(void)
	: _buffer(new char[1]),
	  _buffer_len(1),
	  _length_str(0)
{
	_buffer[0] = '\0';
}

string::string(size_t length_str)
	: _buffer(new char[length_str + 1]),
	  _buffer_len(length_str + 1),
	  _length_str(0)
{
	_buffer[0] = '\0';
}

string::string(char const *str)
{
	_length_str = strlen(str);
	_buffer_len = _length_str + 1;
	_buffer     = new char[_buffer_len];
	memcpy(_buffer, str, _length_str + 1);
}

string::~string(void)
{
	delete[] m_buffer;
}

void string::setString(char const *str)
{
	_length_str = strlen(str);

	// Check for a buffer overflow from the new string.
	if (_length_str + 1 > _buffer_len) {
		_length_str = _buffer_len - 1;
	}
	memcpy(_buffer, str, _length_str + 1);
}

void string::setMaxLength(uint16_t length_str)
{
	// Expand the buffer without losing its content.
	if (length_str > _buffer_len) {
		char *buffer_new = new char[length_str + 1];
		memcpy(buffer_new, _buffer, _buffer_len);
		delete[] _buffer;
		_buffer = buffer_new;
	}

	// Force the smaller buffer to be null-terminated.
	if (length_str < _length_str) {
		_buffer[length_str] = 0;
		_length_str         = length_str;
	}

	_buffer_len = length_str + 1;
}

size_t string::serialize(uint8_t *buffer)
{
	uint32_t length_str = (uint32_t)_length_str;
	memcpy(buffer, &length_str, sizeof(uint32_t));
	memcpy(buffer + 4, _buffer, _length_str);
	return _length_str + 4;
}

size_t string::deserialize(uint8_t const *buffer)
{
	uint32_t length_str;
	memcpy(&length_str, buffer, sizeof(uint32_t));
	_length_str = (size_t)length_str;

	// Quietly deal with overflow by taking as much as possible.
	if (length + 1 > _buffer_len) {
		_length_str = _buffer_len - 1;
	}

	memcpy(_buffer, buffer, _length_str + 1);
	_buffer[_length_str] = '\0';
	return length_str + 4;
}

size_t string::bytes()
{
	return _length_str + 4;
}

