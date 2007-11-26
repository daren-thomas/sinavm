#!/usr/bin/python

'''testharness.py
   test the two garbage collectors, timing them for different
   heap sizes, line sizes and input file sizes
'''

import os, time

heap_sizes = [2**n * 1024 for n in range(1, 16)]

# input files vary in linecount and linelength
LINE_COUNT_SMALL, LINE_COUNT_BIG = 1000, 10000
LINE_SIZE_SMALL, LINE_SIZE_MEDIUM, LINE_SIZE_BIG = 10, 100, 500
files = [
    (LINE_COUNT_SMALL, LINE_SIZE_SMALL),
    (LINE_COUNT_SMALL, LINE_SIZE_MEDIUM),
    (LINE_COUNT_SMALL, LINE_SIZE_BIG),
    (LINE_COUNT_BIG,   LINE_SIZE_SMALL),
    (LINE_COUNT_BIG,   LINE_SIZE_MEDIUM),
    (LINE_COUNT_BIG,   LINE_SIZE_BIG)
]

allocators = ['concurrent', 'twospace']

make_command = 'make HEAP_SIZE=%s %s'
run_command = 'cat %s | ./sina cesar_shift.sina'
input_file = 'input.txt'

def time_execution(allocator, file, heap):
    os.system("make clean")
    result = os.system(make_command % (heap, allocator))
    if result:
        # wait so user can read error messages
        raw_input("Please continue with [ENTER]")
    start_time = time.time()
    result = os.system(run_command % input_file)
    end_time = time.time()
    if result == 0:
        return end_time - start_time
    else:
        # wait so user can read error messages
        raw_input("Please continue with [ENTER]")
        return None

def create_file(line_count, line_size):
    f = open(input_file, 'w')
    c = 0 
    for i in range(line_count):
        f.write(str(i))
        for j in range(line_size - len(str(i))):
            f.write(chr(ord('a') + c))
            c = (c + 1) % 24
        f.write('\n')

if __name__ == '__main__':
    for file in files:
        create_file(file[0], file[1]) 
        for allocator in allocators:
            for heap in heap_sizes:
                print "runnung: %s, %s, %s\n" % (allocator, file, heap)
                t = time_execution(allocator, file, heap)
                result_file = open('results.csv', 'a')
                result_file.write("%s, %s, %s, %s\n" % (
                    allocator, file, heap, t))
                result_file.close()
