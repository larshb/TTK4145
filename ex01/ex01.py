from threading import Thread

i = 0

def thread_1_func():
	global i
	for j in range(1000000):
		i+=1


def thread_2_func():
	global i
	for j in range(1000000):
		i-=1


def main():
	thread_1 = Thread(target = thread_1_func, args = (),)
	thread_2 = Thread(target = thread_2_func, args = (),)
	thread_1.start()
	thread_2.start()
	thread_1.join()
	thread_2.join()
	print(i)


main()
