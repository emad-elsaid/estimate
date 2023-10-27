#include "helpers.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *h(char *input) {
  int length = 0;
  int to_escape = 0;
  for (char *c = input; *c != 0; c++) {
    length++;
    if (*c == '<' || *c == '>')
      to_escape++;
  }

  int to_increase = to_escape * 3; // each encoding increase string length by 3
  int new_length = length + to_escape;

  char *out = (char *)calloc(1, new_length + 1);
  for (char *c = input, *d = out; *c != 0; c++,d++) {
    if (*c == '<' ){
      strcat(d, "&lt;");
      d += 3;
    } else if (*c == '>') {
      strcat(d, "&gt;");
      d += 3;
    } else {
      *d = *c;
    }
  }

  return out;
}

#define maxIntToChar 100
char *longToChar(long i) {
  static char *s[maxIntToChar + 1] = {
      "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  "10", "11",
      "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
      "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35",
      "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47",
      "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
      "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71",
      "72", "73", "74", "75", "76", "77", "78", "79", "80", "81", "82", "83",
      "84", "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95",
      "96", "97", "98", "99", "100"};
  if (i > maxIntToChar)
    return s[maxIntToChar];

  return s[i];
}

char *intToChar(int i) {
  return longToChar(i);
}

char *timeToChar(time_t t){
  return ctime(&t);
}
