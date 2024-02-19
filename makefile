concurrentqueue:
	g++ -fopenmp concurrentqueue.cpp universal.h

concurrentstack:
	g++ -fopenmp concurrentstack.cpp universal.h

debug_concurrentqueue:
	g++ -g -fopenmp concurrentqueue.cpp universal.h 

debug_concurrentstack:
	g++ -g -fopenmp concurrentstack.cpp universal.h