#!/usr/bin/python
import string
import platform
import types

def get_content(filename, outputfile):
	f = open(filename, "r")
	fp = open(outputfile, "w")
	content = []
	proposal = 0
	disclosure = 0
	under_search = 0
	published = 0
	filed = 0
	in_range = False
	for line in f:
		if string.find(line, "Idea Track Table") != -1:
			in_range = True 
		if string.find(line, "*Status should be one") != -1:
			in_range = False 
		if in_range == True:
			#print line,
			if string.find(line, "Proposal") != -1:
				proposal += 1
			elif string.find(line, "Disclosure") != -1:
				disclosure += 1
			elif string.find(line, "Under Search") != -1:
				under_search += 1
			elif string.find(line, "Published") != -1:
				published += 1
			elif string.find(line, "Filed") != -1:
				filed += 1
	
	#print "%d %d %d %d %d " %(proposal, disclosure, under_search, published, filed)
	fp.write("-----------------------------------------------------------------------------------------\n")
	fp.write("| 1st Manager| Proposal | Disclosure | Under Search | Published | Filed                 |\n")
	fp.write("-----------------------------------------------------------------------------------------\n")
	fp.write("| HongBing   | %d        | %d          | %d            | %d         | %d                     |\n"% (proposal, disclosure, under_search, published, filed))
	fp.write("-----------------------------------------------------------------------------------------\n")
			
	#contest.append("a")
	f.close()
	fp.close()
	return 0

if __name__ == "__main__":
	get_content("Patent_status_2012", "patent_output")
