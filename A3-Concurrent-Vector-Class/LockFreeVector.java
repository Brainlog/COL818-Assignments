import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.atomic.AtomicReferenceArray;

public class LockFreeVector<T> {
	
	private static class WriteDescriptor<T> {
		T oldValue, newValue;
		int idx;
		boolean pending;

		WriteDescriptor(T _oldV, T _newV, int pos) {
			newValue = _newV;
			oldValue = _oldV;
			idx = pos;
			pending = true;
		}
	}

	private static class Descriptor<T> {
		int size;
		WriteDescriptor<T> write_operation;

		Descriptor(int _siz, WriteDescriptor<T> _write_1) {
			size = _siz;
			write_operation = _write_1;
		}
	}

	AtomicReferenceArray<AtomicReferenceArray<T>> memory;
	AtomicReference<Descriptor<T>> descriptor;
	int First_Bucket_Size = 2; 

	public LockFreeVector() {
		descriptor = new AtomicReference<Descriptor<T>>(new Descriptor<T>(0, null));
		memory = new AtomicReferenceArray<AtomicReferenceArray<T>>(32);
		memory.getAndSet(0, new AtomicReferenceArray<T>(First_Bucket_Size));
	}
	
	public LockFreeVector(int size) {
		this();
		reserve(size);
		descriptor.get().size = size;	
	}

	void reserve(int change_size) {
		int index = Bucketnumber(descriptor.get().size - 1);
		if (index < 0) index = 0;
		while (index < Bucketnumber(change_size - 1)) {
			index++;
			allocBucket(index);
		}
	}

	void push_back(T elem_latest) {
		Descriptor<T> Desc_current, Desc_latest;
		do {
			Desc_current = descriptor.get();
			completeWrite(Desc_current.write_operation);
			int bucketIdx = highestBit(Desc_current.size + First_Bucket_Size) - highestBit(First_Bucket_Size);
			if (memory.get(bucketIdx) == null) allocBucket(bucketIdx);
			WriteDescriptor<T> write_operation = new WriteDescriptor<T>(read(Desc_current.size), elem_latest, 
					Desc_current.size);
			Desc_latest = new Descriptor<T>(Desc_current.size + 1, write_operation);
		} while (!descriptor.compareAndSet(Desc_current, Desc_latest));
		completeWrite(Desc_latest.write_operation);
	}

	T pop_back() {
		Descriptor<T> Desc_latest, Desc_current;
		T elem;
		do {
			Desc_current = descriptor.get();
			completeWrite(Desc_current.write_operation);	
			if (Desc_current.size == 0) return null; 
			elem = read(Desc_current.size - 1);
			Desc_latest = new Descriptor<T>(Desc_current.size - 1, null);
		} while (!descriptor.compareAndSet(Desc_current, Desc_latest));
		
		return elem;
	}
	
	void write(int idx, T newValue) {
		memory.get(Bucketnumber(idx)).set(pos_in_bucket(idx), newValue);
	}

	T read(int idx) {
		return memory.get(Bucketnumber(idx)).get(pos_in_bucket(idx));
	}

	int size() {
		Descriptor<T> Desc_current = descriptor.get();
		int size = Desc_current.size;
		if (Desc_current.write_operation != null && Desc_current.write_operation.pending) { 
			size--;
		}
		return size;
	}

	private void allocBucket(int bucketIdx) {
		int bucketSize = 1 << (bucketIdx + highestBit(First_Bucket_Size));
		AtomicReferenceArray<T> newBucket = new AtomicReferenceArray<T>(bucketSize);
		if (!memory.compareAndSet(bucketIdx, null, newBucket)) {
		}
	}

	private void completeWrite(WriteDescriptor<T> write_operation) {
		if (write_operation != null && write_operation.pending) {
			memory.get(Bucketnumber(write_operation.idx)).compareAndSet(pos_in_bucket(write_operation.idx), 
					write_operation.oldValue, write_operation.newValue);
			write_operation.pending = false;
		}
	}



	// Bucket changes 
	private int pos_in_bucket(int i) {
		int pos = i + First_Bucket_Size;
		int hibit = highestBit(pos);
		return pos ^ (1 << hibit);
	}

	private int Bucketnumber(int i) {
		int pos = i + First_Bucket_Size;
		int hibit = highestBit(pos);
		return hibit - highestBit(First_Bucket_Size);
	}
	private int highestBit(int n) {
		return Integer.numberOfTrailingZeros(Integer.highestOneBit(n));
	}

}
