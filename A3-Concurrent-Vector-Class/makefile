lock_free_vector_java:
	javac LockFreeVector.java DriverLFVector.java
	java DriverLFVector
lock_free_vector_cpp:
	g++ -fopenmp -std=c++11 -o lock_free_vector_cpp lockfree_vector.cpp
	./lock_free_vector_cpp
stl_mutex_vector_cpp:
	g++ -fopenmp -std=c++11 -o stl_mutex_vector_cpp Driver_STL_Mutex_vector.cpp
	./stl_mutex_vector_cpp
clean:
	rm -f *.class lock_free_vector_cpp stl_mutex_vector_cpp