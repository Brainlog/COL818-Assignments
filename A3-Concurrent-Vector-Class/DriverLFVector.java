import java.time.LocalTime;
import java.io.PrintWriter;
public class DriverLFVector {
    private static final LockFreeVector<Integer> vector = new LockFreeVector<>();

    public static void main(String[] args) {
        int numThreads = 4;
        Thread[] threads = new Thread[numThreads];
        long startT = System.currentTimeMillis();
        for (int i = 0; i < numThreads; i++) {
            threads[i] = new Thread(() -> {
                for (int j = 0; j < 100000; j++) {
                    vector.push_back(j);
                }
                for (int j = 0; j < 1000; j++) {
                    vector.pop_back();
                }
            });
            threads[i].start();
        }

        for (Thread thread : threads) {
            try {
                thread.join();
                
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        long endT = System.currentTimeMillis();
        System.out.println(vector.size());
        long timeTaken = endT - startT;
        try{
            PrintWriter writer = new PrintWriter("LockFreeVectorOutput.txt");
            writer.println("Num threads: " + numThreads);
            writer.println("Time taken: " + timeTaken + "ms");
            writer.close();
            System.out.println("Output written to file: " + "LockFreeVectorOutput.txt");
        }
        catch(Exception e){
            System.out.println("Error in writing to file");
        }

        // System.out.println("Time taken: " + timeTaken + "ms");
    }
}