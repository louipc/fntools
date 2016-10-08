/**
 *  fntools is Copyright © 2016 Loui Chang
 *
 *  fntools are licensed under a Creative Commons Attribution-Share Alike 3.0
 *  United States License.
 */
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Canadian SIN numbers attributes */
#define SINLEN 9
#define SEP  '-'
const uint8_t VALID_PREFIX[] = {1, 2, 3, 4, 5, 6, 7, 9};

/* Pick randomish digit from 0 to divisor */
uint64_t rand_digit(int divisor) {
    struct timespec times;
    clock_gettime(CLOCK_REALTIME, &times);
    srand(times.tv_nsec);
    long randi = rand();

    return randi % divisor;
}

/*
 * Convert a string into an array of ints
 * Extract numerical digits from a string in sequential order.
 */
uint8_t gimme_digits(uint8_t buf[], char *str) {
    uint8_t len = strlen(str);

    uint8_t i = 0;
    uint8_t j = 0;

    do {
        if ( '0' <= str[i] && str[i] <= '9') {
            buf[j] = str[i] - '0';
            j++;
        }
        i++;
    } while ( i < len );

    return j;
}

/* Validate using the Luhn Algorithm (MOD10)
 * See: http://en.wikipedia.org/wiki/Luhn_algorithm
 */
bool luhn(const uint8_t *nums, uint8_t len) {
    bool odd = !(len % 2);
    uint16_t sum = 0;

    for (int i = 0; i < len; ++i) {
        int n = 0 + nums[i];
        odd = !odd;
        if (odd) {
            sum += n;
        }
        else {
            int x = 2 * n;
            sum += (x > 9)? x - 9: x;
        }
    }
    return ((sum % 10) == 0);
}

bool validate_sin(uint8_t nums[], uint8_t len) {
    if ( len == SINLEN ) {
        /* Invalid prefix */
        if (nums[0] == '0' || nums[0] == '8') {
            return false;
        }

        return luhn(nums, len);
    }

    return false;
}

void generate_sin(uint8_t *buf) {

    /* Pick one random element for first digit of sin */
    buf[0] = VALID_PREFIX[rand_digit(8)];

    for (uint8_t i = 1; i < SINLEN-1; i++) {
        /* Add next random number to sin */
        buf[i] = rand() % 9;
    }

    /* reverse the numbers */
    uint8_t reversed[SINLEN];
    for (int8_t i = SINLEN-2; i >= 0; i--) {
        reversed[SINLEN-2-i] = buf[i];
    }

    uint8_t sum = 0;
    uint8_t pos = 0;

    while (pos < SINLEN - 1) {
        uint8_t odd = reversed[pos] * 2;
        if (odd > 9) {
            odd -= 9;
        }

        sum += odd;
        if (pos != (SINLEN - 2)) {
            sum += reversed[pos + 1];
        }
        pos += 2;
    }

    /* Last digit is the check digit */
    buf[SINLEN-1] = (uint8_t) ((floor(sum/10) + 1) * 10 - sum) % 10;
}

void format_sin(char *str, uint8_t sin[]) {
    sprintf(str, "%d%d%d%c%d%d%d%c%d%d%d",
        sin[0], sin[1], sin[2], SEP,
        sin[3], sin[4], sin[5], SEP,
        sin[6], sin[7], sin[8]);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        uint8_t digits[strlen(argv[1])];
        uint8_t len = gimme_digits(digits, argv[1]);

        printf("Input %s \n", argv[1]);

        printf("Using ");

        for (int i = 0; i < len; i++) {
            printf("%d", digits[i]);
        }

        printf(" ... ");

        if (validate_sin(digits, len)) {
            printf("Valid.\n");
            return 0;
        }
        else {
            printf("INVALID.\n");
            return 1;
        }
    }
    else {
        uint8_t sin[SINLEN];
        char str[SINLEN+2+1];
        generate_sin(sin);
        format_sin(str, sin);
        printf("%s\n", str);
    }

    return 0;
}

