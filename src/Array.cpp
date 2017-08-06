#include <iostream>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sstream>
#include <stdint.h>
#include <limits.h>

inline void asmReverse(char * reversed, const char * string, const int stringLength)
{
    __asm__ __volatile__ (
                    " pushq %%rdi\n"
                    " pushq %%rsi\n"
                    " addq %%rax, %%rsi\n"
                    " subq $1, %%rsi\n"
                    " popq %%rax\n"
                    " subq $1, %%rax\n"
                    " pushq %%rsi\n"

                    " loop:\n"
                    " movb (%%rsi), %%bl\n"
                    " subb $32, %%bl\n"
                    " jz spaceFound\n"
                    " subq $1, %%rsi\n"
                    " pushq %%rsi\n"
                    " subq %%rax, %%rsi\n"
                    " jz finalCopy\n"
                    " popq %%rsi\n"
                    " jmp loop\n"

                    " spaceFound:\n"
                    " subq $1, %%rdi\n"
                    " movq $0, %%rdx\n"
                    " copyLoop:\n"
                    " popq %%rcx\n"
                    " pushq %%rcx\n"
                    " subq %%rsi, %%rcx\n"
                    " jz copyDone\n"
                    " addq $1, %%rsi\n"
                    " addq $1, %%rdi\n"
                    " movb (%%rsi), %%bl\n"
                    " movb %%bl, (%%rdi)\n"
                    " addq $1, %%rdx\n"
                    " jmp copyLoop\n"

                    " copyDone:\n"
                    " addq $2, %%rdi\n"
                    " popq %%rcx\n"
                    " subq %%rdx, %%rcx\n"
                    " subq $1, %%rcx\n"
                    " pushq %%rcx\n"
                    " movq %%rcx, %%rsi\n"
                    " jmp loop\n"

                    " finalCopy:\n"
                    " popq %%rsi\n"
                    " subq $1, %%rdi\n"
                    " finalCopyLoop:\n"
                    " popq %%rcx\n"
                    " pushq %%rcx\n"
                    " subq %%rsi, %%rcx\n"
                    " jz end\n"
                    " addq $1, %%rsi\n"
                    " addq $1, %%rdi\n"
                    " movb (%%rsi), %%bl\n"
                    " movb %%bl, (%%rdi)\n"
                    " jmp finalCopyLoop\n"

                    " end:\n"
                    " popq %%rcx\n"
                    " popq %%rdi\n"
                    :
                    : "S" (string), "D" (reversed), "a" (stringLength)
                    : "rbx", "rcx", "rdx"
                    );
}

inline void reverse(char * reversed, const char * string, const int stringLength)
{
    const char * stringStart = &string[0];
    char * marker = (char *)&string[stringLength - 1];
    char * lastMarker = marker;
    char * lastCopyTo = &reversed[0];

    while(marker != stringStart)
    {
        if(*marker == ' ')
        {
            memcpy(lastCopyTo, marker + 1, lastMarker - marker);
            lastCopyTo += (lastMarker - marker) + 1;
            *marker--;
            lastMarker = marker;
        }
        else
        {
            *marker--;
        }
    }

    memcpy(lastCopyTo, marker, lastMarker - marker + 1);
}

int main(int argc, char** argv)
{
    if(argc == 1)
    {
        std::cout << "Run like ./WordReverse <space separated string to reverse>" << std::endl;
        return 0;
    }
    
    std::stringstream inStr;
    for(int i = 1; i < argc; i++)
    {
        inStr << argv[i];
        
        if(i != argc - 1)
        {
            inStr << " ";
        }
    }
    
    std::string input = inStr.str();

    timeval timer;

    const uint64_t iterations = 1000000ull;

    char * reversed = (char *)calloc(input.length(), sizeof(char));
    memset(reversed, (int)' ', sizeof(char) * input.length());

    std::cout << "ASM: ";
    gettimeofday(&timer, 0);
    double start = (double)timer.tv_sec + ((double)timer.tv_usec / double(iterations));
    for(uint64_t i = 0; i < iterations; i++)
    {
        asmReverse(reversed, input.c_str(), input.length());
    }
    gettimeofday(&timer, 0);
    double end = (double)timer.tv_sec + ((double)timer.tv_usec / double(iterations));
    std::cout << (end - start) / double(iterations);
    std::cout << ", reversed: " << reversed << std::endl;

    memset(reversed, (int)' ', sizeof(char) * input.length());
    
    std::cout << "C++: ";
    gettimeofday(&timer, 0);
    start = (double)timer.tv_sec + ((double)timer.tv_usec / double(iterations));
    for(uint64_t i = 0; i < iterations; i++)
    {
        reverse(reversed, input.c_str(), input.length());
    }
    gettimeofday(&timer, 0);
    end = (double)timer.tv_sec + ((double)timer.tv_usec / double(iterations));
    std::cout << (end - start) / double(iterations);
    std::cout << ", reversed: " << reversed << std::endl;
    
    free(reversed);
}
