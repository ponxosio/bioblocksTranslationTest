import sys

def runSimulation(file, timeSlice, units) :
	globalTime = 0;
	f = open(file,'r')
	
	for line in f:
		if line == "timeStep();\n" :
			globalTime = globalTime + timeSlice;
		else :
			print str(globalTime) + units + ":" + line[0:-1];
	f.close()

if __name__ == "__main__":
	runSimulation(sys.argv[1], int(sys.argv[2]), sys.argv[3])