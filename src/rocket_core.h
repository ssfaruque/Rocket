#ifndef ROCKET_CORE_H
#define ROCKET_CORE_H

#include <assert.h>

#include "page.h"

#define ASSERT(cond, str) {assert((cond) && (str));}

enum Operation
{
    WRITING = 1,
    READING = 2,
    NONE = 0
};


void* get_base_address();

PageOwnership* create_pageownerships(int number_of_pages, int number_of_clients);

#endif  // ROCKET_CORE_H