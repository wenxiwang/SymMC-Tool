import os
import signal
import sys
import subprocess

root = getcwd()
executable = root + '/Enumerator_Estimator/cmake-build-release/minisat'
timeout = 5000
files =[]

def recursiveopen(loc):
	for file in os.listdir(loc):
		if file == '..' or file == '.':
			continue
		if os.path.isdir(loc+file+"/"):
			recursiveopen(loc+file+"/")
		elif os.path.isfile(loc+file) and file.endswith(".cnf"):
			files.append(loc+file)

recursiveopen(sys.argv[1])

#print(files)
for cnff in files:
	permf = cnff.replace("cnf", "perm")
	sys.stdout.flush()
	sys.stderr.flush()
	l = [executable,cnff,permf]
	sys.stdout.write(str(cnff)+",") #print test name
	sys.stdout.flush()
	sys.stderr.flush()
	pro = subprocess.Popen(l)
	try:
		pro.wait(timeout)
		sys.stdout.flush()
		sys.stderr.flush()
	except subprocess.TimeoutExpired:
		pro.kill()
		print("unknown,5000")
		sys.stdout.flush()
		sys.stderr.flush()
