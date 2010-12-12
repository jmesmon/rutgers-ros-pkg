#!/usr/bin/env python
# vim: ts=4:sw=4:noet:ai

"""
AVR code generator for ROS topics.  This generates AVR source code
so that the avr can communicate with the

Converts ROS .msg files in a package into Python source code implementations.

arrays have an unsigned integer specifying the number of units in the array
"""
import roslib; roslib.load_manifest('avr_bridge')

import sys
import shutil
import os
import traceback

# roslib.msgs contains the utilities for parsing .msg specifications.
# It is meant to have no rospy-specific knowledge
import roslib.msgs
import roslib.packages


import roslib.genpy
import yaml
import StringIO

primatives = {
	'bool'  :   ('bool', 1) ,
	'byte'  :   ('uint8_t', 1),
	'int8'  :   ('uint8_t', 1),
	'int16' :   ('int16_t', 1),
	'uint16':   ('uint16_t', 2),
	'int32':    ('int32_t', 4),
	'uint32':   ('uint32_t', 4),
	'int64':    ('int64_t',8),
	'uint64':   ('uint64_t',8),
	'float32':  ('float', 4 ),
	'float64':  ('double', 8),
	'time':     ('int64_t',8),
	'duration': ('int64_t',8),
	'string' :  ('ROS::string', 0)
}

def extract_ros_type(field):
	""" This is an utility function to extract the basic type
	    from the msg_spec field
	    without the array symbols
	    and with the pkg sepparted
	"""
	try:
		incPkg, incName = field.type.split('/')
	except:
		incPkg = None
		incName = field.type
	if field.is_array:
		incName = incName[:incName.find('[')]
	return incPkg, incName

def serialize_primative(f, buffer_addr, field):
	"""
	Generates c code to serialize rostype of fieldname at the buffer
	"""
	fpkg, ftype = extract_ros_type(field)
	ctype, clen = primatives[ftype]


	if (field.is_array or field.type == 'string'):
		f.write('offset += this->%s.serialize(%s + offset);\n'%(field.name, buffer_addr))
	else:
		f.write("*( (%s *) (%s + offset))=  this->%s; \n"%(ctype, buffer_addr, field.name) )
		f.write('offset += %d;\n'%(clen))


def deserialize_primative(f, buffer_addr, field):
	"""
	Generate c code to deserialize a rosmsg field of type ctype from
	specified buffer.
	"""
	fpkg, ftype = extract_ros_type(field)
	ctype, clen = primatives[ftype]

	if (field.is_array or field.type == 'string'):
		f.write('offset+= this->%s.deserialize(%s+offset);\n'%(field.name, buffer_addr))
	else:
		f.write( "this->%s = *( (%s *) (%s + offset) );\n"%(field.name, ctype, buffer_addr) )
		f.write('offset += %d;\n'%(clen))


def write_header_file(f, msg_name, pkg, msg_spec):
	""" f is a file like object to which the header file is being outputed
		msg_name is the name of the msg
		pkg is the msg's pkg
		includes is a list of file names that should be included
		filds is a list of touples of field type, name, array, array_length
	"""
	#write comments
	f.write("""
/* %s.h
 * This msg file was autogenerated by the Rutgers avr_bridge software.
 */

"""%msg_name)

	#write header guards
	guard =  msg_name+'_H'
	guard = guard.upper()
	f.write("""
#ifndef %s
#define %s
#include "Msg.h"
#include "vector.h"
#include "ros_string.h"

"""%(guard, guard))

	#write includes
	for field in msg_spec.parsed_fields():
		if not field.is_builtin:
			incPkg, incName = extract_ros_type(field)
			f.write('#include "%s.h"\n'%(incName))
	

	
	#open namespace
	f.write('\n\nnamespace %s {\n'%pkg)
	

	f.write("class %s : public Msg{\n   public:\n"%msg_name)
	
	f.write('\t%s();\n'%msg_name);
	f.write('\t%s(uint8_t * data);\n'%msg_name);
	f.write('\t~%s();\n'%msg_name);
	f.write("""
	virtual uint16_t bytes();
	virtual uint16_t serialize(uint8_t * out_buffer);
	virtual uint16_t deserialize(uint8_t * data);

""")

	#write msg fields
	for field in msg_spec.parsed_fields():
		fpkg, ftype = extract_ros_type(field)
		if fpkg != None:
			ftype = fpkg +"::"+ftype
		if ftype == 'Header':
			ftype = "roslib::"+ftype
		if field.is_builtin:
			ftype, clen = primatives[ftype]
		if field.is_array:
			if field.array_len:
				f.write('\tROS::vector<%s> %s; //fixed at length %d\n'%(ftype, field.name, field.array_len))
			else :
				f.write('\tROS::vector<%s> %s;\n'%(ftype, field.name))
		else:
				f.write('\t%s %s;\n'%(ftype, field.name) )

	f.write('private:\n')

	f.write('}; /* class %s */\n\n'%msg_name)

	#close namespace
	f.write('} /* namespace %s */\n\n'%pkg)
	#closing header guards
	f.write('#endif  /* %s */'%guard)

def serialize_msg(f, msg_spec):
	f.write('\nint offset=0;\n')

	for field in msg_spec.parsed_fields():
		if (field.is_builtin):
			serialize_primative(f, 'data', field)
		else:
			f.write('offset += this->%s.serialize(%s + offset);\n'%(field.name, 'data'))

	f.write('\n return offset;\n')

def deserialize_msg(f, msg_spec):
	f.write('\nint offset=0;\n')

	for field in msg_spec.parsed_fields():
		if (field.is_builtin):
			deserialize_primative(f, 'data', field)
		else:
			f.write('offset += this->%s.deserialize(%s + offset);\n'%(field.name, 'data'))

	f.write('\n return offset;\n')



def msg_size(f, msg_spec):
	f.write('\n int msgSize=0;\n')

	for field in msg_spec.parsed_fields():
		if (field.is_builtin and not (field.type == 'string') ):
			fpkg, ftype = extract_ros_type(field)
			ctype, clen = primatives[ftype]
			f.write('msgSize += sizeof(%s);\n'%(ctype))
		else:
			f.write('msgSize += %s.bytes();\n'%field.name)

	f.write('return msgSize;\n')


def write_cpp(f, msg_name, pkg, msg_spec):
	f.write('#include "%s.h"\n'%msg_name)
	f.write('#include <stdio.h>\n')

	f.write('using namespace %s ;\n'%pkg)

	def writeFunct(rtype, msg, funct, args, implementation):
		f.write('\n%s %s::%s(%s){'%(rtype,msg,funct,args))
		implementation()
		f.write('\n}\n')

	#write constructor
	f.write('\n%s::%s() '%(msg_name, msg_name))
	#set fixed length arrays
	constructor_init= ''
	for field in msg_spec.parsed_fields():
		if field.is_array:
			if field.array_len:
				constructor_init +='%s(%d),' %(field.name, field.array_len)
	if (len(constructor_init) > 3):
		f.write( ' : ' + constructor_init[:-1])
	f.write('{} \n\n')


	writeFunct('', msg_name, '~'+msg_name, '',lambda : 0)

	writeFunct('', msg_name, msg_name,'uint8_t* data', lambda : '\nthis->deserialize(data);\n')

	writeFunct('uint16_t ', msg_name, 'serialize', 'uint8_t* data', lambda :serialize_msg(f, msg_spec))
	writeFunct('uint16_t ', msg_name, 'deserialize', 'uint8_t* data', lambda : deserialize_msg(f,msg_spec))
	writeFunct('uint16_t ', msg_name, 'bytes', '', lambda : msg_size(f, msg_spec))



class CGenerator():
	def __init__(self):
		self.types = [] #contains list of distinct types
						#each type will generate a .h and .cpp file
		self.msg_specs = {} #dict containing the msg spec for each type

		self.topicIds = {}
		self._topicIds = 0

		self.config = None

	def parseConfig(self, configFile):
		#takes a file like object of configuration yaml

		self.config = yaml.load(configFile)

		#services get their topic id first
		if self.config.has_key('service'):
			for topic in self.config['service']:
				#import that msg's python module
				msg_type = self.config['service'][topic]['type']

			#TODO IMPLEMENT SERVICES

				self.topicIds[topic] = self._topicIds
				self._topicIds +=1


		#subscribes must get their topic id first
		if self.config.has_key('subscribe'):
			for topic in self.config['subscribe']:
				#import that msg's python module
				msg_type = self.config['subscribe'][topic]['type']

				self.addMsg(topic, msg_type)

				self.topicIds[topic] = self._topicIds
				self._topicIds +=1


		if self.config.has_key('publish'):
			for topic in self.config['publish']:
				#import that msg's python module
				msg_type = self.config['publish'][topic]['type']

				self.addMsg(topic, msg_type)

				self.topicIds[topic] = self._topicIds
				self._topicIds +=1

	def addMsg(self, pkg, name):
		"""
		@param pkg  The package that the msg is found in
		@param msg  The name of the msg
		"""

		msg_name, msg_spec = roslib.msgs.load_by_type(name, pkg)
		if not (msg_name in self.types):
			self.types.append(msg_name)
			self.msg_specs[msg_name] = msg_spec

			#now that we are done adding the base msg type, we need to
			#recursively add all the types defined within it
			for type in msg_spec.types:
				if type[-1] == ']': #strip away array markers
					type = type[0:type.find('[')]

				if type == 'Header':
					self.addMsg('roslib', 'Header')
				elif primatives.has_key(type) or type== 'string':
					pass
				else:
					tpkg, tmsg = type.split('/')
					self.addMsg(tpkg, tmsg)
	def generateRos(self, folderPath):

		f = open(folderPath+'/ros_generated.cpp', 'w');

		f.write("""
/* AUTOGENERATED by avr_bridge
 *
 * avr_bridge was written by
 * Adam Stambler and Phillip Quiza of Rutgers University.
 */

#include "Ros.h"

uint8_t Ros::getTopicTag(char * topic)
{
""")
		for topic, ID in self.topicIds.iteritems():
			f.write("""
	if (!strcmp(topic, "%s"))
		return %d;
"""%(topic, ID))

			f.write("""
	return 0;
}

""")

		f.write('Ros ros("%s"); //autogenerated Ros object'%self.config['port'])
		f.close()


	def generateMsgFile(self, folderPath, msg):
		pkg, msg_name = msg.split('/')

		header_file = open(folderPath+'/'+msg_name+'.h', 'w')
		write_header_file(header_file, msg_name, pkg, self.msg_specs[msg])
		header_file.close()

		cpp_file = open(folderPath+'/'+msg_name+'.cpp','w')
		write_cpp(cpp_file, msg_name, pkg, self.msg_specs[msg])

	def generate(self, folderPath):
		"""This function generates the ros implementation for the avr code
		"""
		genPath = roslib.packages.find_resource('avr_bridge' ,'gen_avr.py')[0]
		avrRosPath =  genPath[:-len('gen_avr.py')]+ 'avrRos'


		try:
			shutil.copytree(avrRosPath, folderPath+'/avrRos')
		except Exception as e:
			print "avrRos already exists in ", folderPath
			print "The new files are overwriting the old files"

		for t in self.types:
			self.generateMsgFile(folderPath+'/avrRos', t)

		self.generateRos(folderPath+'/avrRos')


def test():
	roslib.msgs.set_verbose(False)

	typeList = []

	msg_name = 'Odometry'
	pkg_name = 'nav_msgs'
	msg_path = roslib.msgs.msg_file(pkg_name, msg_name)
	msg_name, msg_spec = roslib.msgs.load_from_file(msg_path)

	gen = CGenerator()
	gen.addMsg(pkg_name, msg_name)


