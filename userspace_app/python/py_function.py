#!/usr/bin/python
import platform

def multiply():
	c = 12345*6789
	print 'The result of 12345*6789 :', c
	return c

if __name__ == "__main__":
	multiply()
	print platform.machine()
