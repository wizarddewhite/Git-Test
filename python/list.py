#!/usr/bin/python
if __name__ == "__main__":
    d = {}
    li = ["Mon", "Tue" ]
    Tu = (1, 2, 3 )
    li.append("Wen")
    for i in range(0,3): 
        d[Tu[i]] = li[i]
    print d
