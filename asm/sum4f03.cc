//! -O3

int sum4(int* array) {
	int sum = 0;
	for (int i = 0; i != 4; ++i) {
		sum += array[i];
	}
	return sum;
}
