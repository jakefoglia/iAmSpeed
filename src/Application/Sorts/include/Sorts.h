#include <tuple>
#include <functional>


void insertion_sort(int* array, int num_elements);

void heapify_iterative(int* array, int num_elements, int position);

void heapify_recursive(int* array, int num_elements, int position);

void heap_sort_iterative(int* array, int num_elements);

void heap_sort_recursive(int* array, int num_elements);

void merge_sort_buffered_internal(int* array, int num_elements, int* temp_buffer);

void merge_sort_buffered(int* array, int num_elements);

void quick_sort(int* array, int num_elements);

void radix_sort(int* array, int num_elements);

void std_sort(int* array, int num_elements);

void print_array(int* array, int num_elements);

bool verify_sorted(int* array, int num_elements);

std::tuple<bool, double> test(int* source_array, int* array, int num_elements, bool verify, const char* func_name, std::function<void(int*, int)> sort_func);

