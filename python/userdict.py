#!/usr/bin/python
# Filename: inherit.py

import UserDict, cPickle

class Env(UserDict.IterableUserDict):
	def __init__(self, filename=None, version=0):
		UserDict.IterableUserDict.__init__(self)
		empty = {"version": version}

		if filename:
			self._filename = filename
			try:
				if os.path.isfile(filename):
					f = open(filename, "r")
					env = cPickle.load(f)
					f.close()
					if env.get("version", 0) >= version:
						self.data = env
					else:
						self.data = empty
				else:
					self.data = empty
			except:
				self.data = empty
		else:
			self.data = empty

	def save(self, filename=None):
		filename = filename or self._filename
		f = open(filename, "w")
		cPickle.dump(self.data, f)
		f.close()

def main():
	print "userdic example"
	env = Env("env");
	env["test"] = "change"
	print env
	env.save()

if __name__ == "__main__":
	main()
