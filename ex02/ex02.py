
import threading
from threading import Thread

i = 0
lock = threading.Lock()
def Thread1():
	global i
	for r in range(0,1000000):
		lock.acquire()
		try:
			i = i + 1
		finally:
			lock.release()	

def Thread2():
	global i
	for t in range(0,1000000):
		lock.acquire()
		try:
			i = i - 1
		finally:
			lock.release()	
		

def main():
	thread1 = Thread(target = Thread1, args = (),)
	thread2 = Thread(target = Thread2, args = (),)
	thread1.start()
	thread2.start()
	thread1.join()
	thread2.join()
	print(i)

main()
