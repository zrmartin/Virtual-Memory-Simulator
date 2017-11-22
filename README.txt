https://github.com/zrmartin/Virtual-Memory-Simulator

$ ./vmsim [bus] [vm] [phys]

$ [r/w] [vm address request1]

$ [r/w] [vm address request2]

…

[bus] – the VM bus size in bits

[vm] – the number of bits used to represent a virtual page

[phys] – the number of bits used to address physical page frames on the system.

[r/w] – a binary marker is the memory access to to read or write. The pages fetched for read requests can be evicted from the working memory without writes, write requests must be first written to disk before a page fault request is fetched.

[vm address request] – read in as a 32 bit integer, then cast to [bus] size by masking off the upper (the most significant bits) 32-[bus] number of address bits. If the value virtual memory address request value is greater then a maximum value that can be represented by 32bit integer, write an error message.