import sys 
args = sys.argv

f = open(args[1], "w")
print("0"*int(args[2]), file=f)
f.close()
