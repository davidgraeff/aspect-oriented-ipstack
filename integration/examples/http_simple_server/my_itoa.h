#pragma once 

/**
  * Convert integer to string
  */
int my_itoa(int val, char* buf)
{
    char* p = buf;

	// negativ?: add - sign
    if (val < 0) {
        *p++ = '-';
        val = 0 - val;
    }

	// convert digit per digit to ascii chars
    char* b = p;
    unsigned int digit;
    unsigned int restVal = (unsigned int)val;
    do {
        digit = restVal % 10;
        restVal /= 10;
        *p++ = digit + '0'; // make ascii char out of digit
    } while (restVal > 0);

    int len = (int)(p - buf);

	// last character is 0 (decrease buffer pointer after that)
    *p-- = 0;

    // swap
    char temp;
    do {
        temp = *p;
        *p = *b;
        *b = temp;
        --p;
        ++b;

    } while (b < p);

    return len;
}
