concurrentqueue:
	g++ -O0 -fopenmp concurrentqueue.cpp universal.h

concurrentstack:
	g++ -O0 -fopenmp concurrentstack.cpp universal.h

debug_concurrentqueue:
	g++ -g -O0 -fopenmp concurrentqueue.cpp universal.h 

debug_concurrentstack:
	g++ -g -O0 -fopenmp concurrentstack.cpp universal.h