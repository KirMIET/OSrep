#include "find_min_max.h"

#include <limits.h>

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  unsigned int start = begin;
  if ((end - begin) % 2 == 1) {
      min_max.min = array[begin];
      min_max.max = array[begin];
      start = begin + 1;
  }

  // Обрабатываем элементы попарно
  for (unsigned int i = start; i < end; i += 2) {
      if (array[i] < array[i + 1]) {
          if (array[i] < min_max.min) {
              min_max.min = array[i];
          }
          if (array[i + 1] > min_max.max) {
              min_max.max = array[i + 1];
          }
      } else {
          if (array[i + 1] < min_max.min) {
              min_max.min = array[i + 1];
          }
          if (array[i] > min_max.max) {
              min_max.max = array[i];
          }
      }
  }

  return min_max;
}
