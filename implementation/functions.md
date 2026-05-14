## fread
signature: 
```c
#include <stdio.h>

size_t fread(
    void *ptr,
    size_t size,
    size_t nmemb,
    FILE *stream
)
```
- `ptr`: pointer to the buffer where the read data will be stored.
- `size`: the syze in bytes of each element to read
- `nmemb`: the number of elements to read
- `stream`: pointer to the `FILE` object 
- `size_t`: unsigned integer type