/*
 * string.h
 *

# Software License Agreement (BSD License)
#
# Copyright (c) 2011, Adam Stambler
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
#  * Neither the name of Adam Stambler, Inc. nor the names of its
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
