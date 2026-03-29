/*
 * OSAL mock — public API for test harnesses.
 */
#ifndef OSAL_MOCK_H
#define OSAL_MOCK_H

#include <stdio.h>

/* Set the trace output file. All traced OSAL calls go here. */
void osal_mock_set_trace(FILE *fp);

#endif /* OSAL_MOCK_H */
