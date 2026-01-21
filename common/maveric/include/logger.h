/*
 * logger.h
 *
 * Created on: Aug 19, 2025
 *     Author: Adhit Siripurapu 
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "cirbuf.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#define LOG_PREFIX_FMT			"%.3fs -- [%s] "

typedef enum {
    LL_TRACE = 0,
	LL_DEBUG,
    LL_INFO,
    LL_WARN,
    LL_ERROR
} LogLevel;

typedef struct {
    uint8_t stream;
	circbuf_s q;
} logger;

void logger_init(logger* logger, uint8_t stream);
void logger_log_impl(logger* logger, LogLevel level, const char* fmt, va_list args);
void logger_flush(logger* logger);

void log_error(logger* logger, const char* fmt, ...);
void log_warn(logger* logger, const char* fmt, ...);
void log_info(logger* logger, const char* fmt, ...);
void log_debug(logger* logger, const char* fmt, ...);
void log_trace(logger* logger, const char* fmt, ...);

#endif /* __LOGGER_H__ */
