#include <tuple>
#include <functional>
#include <chrono>
#include <stack>
#include <iostream>
#include <algorithm>
#include <cstring>

#include "Sorts.h"

std::tuple<bool, double> test(int* source_array, int* array, int num_elements, bool verify, const char* func_name, std::function<void(int*, int)> sort_func)
{
    //std::cout << "\nsorting " << num_elements << " elements using " << func_name << "..." << std::endl;
    std::memcpy(array, source_array, num_elements * sizeof(int));

    //print_array(array, num_elements);

    auto start = std::chrono::high_resolution_clock::now();
    sort_func(array, num_elements);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;

    bool passed = !verify || verify_sorted(array, num_elements);

    //std::tuple<bool, double> result {passed, diff.count()};
    return { passed, diff.count() };

    //std::cout << (passed ? "passed in " : "failed in ") << diff.count() << "s" << std::endl;

    //print_array(array, num_elements);

}



void insertion_sort(int* array, int num_elements)
{
    for (int i = 1; i < num_elements; i++)
    {
        int key = array[i];
        int j = i - 1;

        while (j >= 0 && array[j] > key)
        {
            // push to the right
            array[j + 1] = array[j];

            // advance
            j--;
        }

        // insert key into correct position of the sorted portion
        array[j + 1] = key;

    }
}

void heapify_iterative(int* array, int num_elements, int position) // slower since we use a stack which allocates on heap, but can theoretically handle more elements
{
    std::stack<int> s;

    int first_leaf_index = num_elements / 2;
    int index = position;

    do // assume original position isnt a leaf node. is this safe based on our caller? 
    {

        if (index < first_leaf_index)
        {
            int value = array[index];

            int lci = index * 2 + 1;
            int rci = index * 2 + 2;

            if (rci < num_elements) // must consider both children
            {
                int lcv = array[lci];
                int rcv = array[rci];

                // find greater of the two children
                int gci = lci;
                if (lcv < rcv)
                {
                    gci = rci;
                }
                int gcv = array[gci];

                // check if we need to swap with the greater child
                if (gcv > value)
                {
                    // swap and recursively reheapify at child
                    std::swap(array[index], array[gci]);
                    //heapify_recursive(array, num_elements, gci);
                    s.push(gci);
                }
            }
            else // if (lci < num_elements) // must only consider left child
            {
                int lcv = array[lci];

                // check if we need to swap with the single child
                if (lcv > value)
                {
                    // swap and recursively reheapify at child
                    std::swap(array[index], array[lci]);
                    //heapify_recursive(array, num_elements, lci);

                    s.push(lci);
                }
            }
        }

        if (s.empty()) {
            index = -1;
        }
        else {
            index = s.top();
            s.pop();
        }

    } while (index != -1);


}

void heapify_recursive(int* array, int num_elements, int position)
{
    int index = position;
    int value = array[position];

    int first_leaf_index = (num_elements / 2);

    // exit recursion if leaf node
    if (index >= first_leaf_index)
    {
        return;
    }

    int lci = position * 2 + 1;
    int rci = position * 2 + 2;

    if (rci < num_elements) // must consider both children
    {
        int lcv = array[lci];
        int rcv = array[rci];

        // find greater of the two children
        int gci = lci;
        if (lcv < rcv)
        {
            gci = rci;
        }
        int gcv = array[gci];

        // check if we need to swap with the greater child
        if (gcv > value)
        {
            // swap and recursively reheapify at child
            std::swap(array[index], array[gci]);
            heapify_recursive(array, num_elements, gci);
        }
    }
    else if (lci < num_elements) // must only consider left child
    {
        int lcv = array[lci];

        // check if we need to swap with the single child
        if (lcv > value)
        {
            // swap and recursively reheapify at child
            std::swap(array[index], array[lci]);
            heapify_recursive(array, num_elements, lci);
        }
    }

}

void heap_sort_iterative(int* array, int num_elements)
{
    /*
    Determining the first non-leaf node based on num_elements

            0
         1     2
        3 4   5 6

    len 7 -> start 2
    len 6 -> start 2
    len 5 -> start 1
    len 4 -> start 1
    len 3 -> start 0
    len 2 -> start 0

    therefore first non-leaf is at (int)(num_elements/2) - 1
    */

    // O(n log n)
    // build max heap
    for (int i = num_elements / 2 - 1; i >= 0; i--)
    {
        heapify_iterative(array, num_elements, i);
    }

    // O(n log n)
    // add top of heap to sorted portion at end and reheapify
    for (int i = num_elements - 1; i > 0; i--)
    {
        std::swap(array[0], array[i]);
        heapify_iterative(array, i, 0);
    }
}



void heap_sort_recursive(int* array, int num_elements)
{
    /*
    Determining the first non-leaf node based on num_elements

            0
         1     2
        3 4   5 6

    len 7 -> start 2
    len 6 -> start 2
    len 5 -> start 1
    len 4 -> start 1
    len 3 -> start 0
    len 2 -> start 0

    therefore first non-leaf is at (int)(num_elements/2) - 1
    */

    // O(n log n)
    // build max heap
    for (int i = num_elements / 2 - 1; i >= 0; i--)
    {
        heapify_recursive(array, num_elements, i);
    }

    // O(n log n)
    // add top of heap to sorted portion at end and reheapify
    for (int i = num_elements - 1; i > 0; i--)
    {
        std::swap(array[0], array[i]);
        heapify_recursive(array, i, 0);
    }
}


void merge_sort_buffered_internal(int* array, int num_elements, int* buffer)
{
    if (num_elements < 2)
    {
        return;
    }

    int half_len = num_elements / 2;

    int* l_array = array;
    int* r_array = array + half_len;

    int l_len = half_len;
    int r_len = (num_elements % 2 == 0) ? half_len : half_len + 1;

    merge_sort_buffered_internal(l_array, l_len, buffer); // left partition
    merge_sort_buffered_internal(r_array, r_len, buffer); // right partition

    int l = 0;
    int r = 0;

    int i = 0;

    // merge into temp_buffer
    while (l < l_len && r < r_len)
    {
        if (l_array[l] < r_array[r])
        {
            buffer[i] = l_array[l++];
        }
        else
        {
            buffer[i] = r_array[r++];
        }
        i++;
    }

    // finish off grabbing from l_array if r_array ran out first
    while (l < l_len)
    {
        buffer[i] = l_array[l++];
        i++;
    }

    // finish off grabbing from r_array if l_array ran out first
    while (r < r_len)
    {
        buffer[i] = r_array[r++];
        i++;
    }

    // put back into original array
    std::memcpy(array, buffer, num_elements * (sizeof(int)));
}

// preallocated shared buffer
void merge_sort_buffered(int* array, int num_elements)
{
    int* buffer = new int[num_elements];
    merge_sort_buffered_internal(array, num_elements, buffer);
    delete[] buffer;
}


int hoare_partition(int* array, int lo, int hi, int pivot_index) {
    int pivot = array[pivot_index];
    int i = lo;
    int j = hi;

    while (true) {

        while (i < j && array[i] < pivot) i++;
        while (i < j && array[j] >= pivot) j--;

        if (i < j) std::swap(array[i], array[j]);
        else return j;
    }
}
void quick_sort_internal(int* array, int lo, int hi)
{
     
    if (lo < hi) {
        int pivot_index = (rand() % (hi - lo + 1)) + lo; // randomized pivot_index is better. should be in range [lo, hi] inclusive 
        //int pivot_index = hi;

        int q = hoare_partition(array, lo, hi, pivot_index);

        if (q == lo) {
            std::swap(array[lo], array[pivot_index]);
            quick_sort_internal(array, lo + 1, hi);

        }
        else {
            quick_sort_internal(array, lo, q - 1);
            quick_sort_internal(array, q, hi);
        }
    }

}

void quick_sort(int* array, int num_elements)
{
    quick_sort_internal(array, 0, num_elements-1);
}

void radix_sort(int* array, int num_elements)
{
    // todo implement
}

void std_sort(int* array, int num_elements)
{
    std::sort(array, array + num_elements);
}

void print_array(int* array, int num_elements)
{
    for (int i = 0; i < num_elements; i++)
    {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

bool verify_sorted(int* array, int num_elements)
{
    for (int i = 0; i < num_elements - 1; i++)
    {
        if (array[i] > array[i + 1])
        {
            return false;
        }
    }
    return true;
}

