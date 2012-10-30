#!/usr/bin/python
import threading, time

class MyThread(threading.Thread):
	def __init__(self, id):
		threading.Thread.__init__(self)
		self.id = id

	def run(self):
		x = 0
		print self.id
		time.sleep(10)
		print "bye the curel world"

if __name__ == "__main__":
	t = MyThread(2)
	t.start()
	print "main"
	for i in range(5):
		print i
