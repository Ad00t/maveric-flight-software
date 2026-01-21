/*
 * logger.c
 * 
 *  Created on: Aug 19, 2025
 *      Author: Adhit Siripurapu 
 */

#include "logger.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

const char* ll_labels[] = {
	"TRACE", "DEBUG", "INFO", "WARN", "ERROR"
};

void logger_init(logger* logger, uint8_t stream) {
	logger->stream = stream;
    cb_init(&logger->q);
}

void logger_log_impl(logger* logger, LogLevel level, const char* fmt, va_list args) {
    if (level < LOG_LEVEL) return;
    float t_s = 0.0f;

    char buf[MAX_BUF_LEN];
    uint8_t max_msg_len = sizeof(buf) - 3;
    int n = snprintf(buf, max_msg_len, LOG_PREFIX_FMT, t_s, ll_labels[level]);
    if (n < 0) n = 0;
    vsnprintf(buf + n, max_msg_len - n, fmt, args);

    uint8_t len = strlen(buf);
    if (len + 2 < sizeof(buf)) {
        buf[len] = '\r';
        buf[len + 1] = '\n';
        buf[len + 2] = '\0';
    } else {
        buf[sizeof(buf) - 3] = '\r';
        buf[sizeof(buf) - 2] = '\n';
        buf[sizeof(buf) - 1] = '\0';
    }

    cb_push()
}

void logger_flush(logger* logger) {
	char buf[logger->msg_size];
	while (osMessageQueueGet(logger->mq_id, buf, NULL, 10) == osOK) {
		fprintf(logger->out, "%s", buf);
	}
}

void log_error(logger* logger, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logger_log_impl(logger, LL_ERROR, fmt, args);
    va_end(args);
}

void log_warn(logger* logger, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logger_log_impl(logger, LL_WARN, fmt, args);
    va_end(args);
}

void log_info(logger* logger, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logger_log_impl(logger, LL_INFO, fmt, args);
    va_end(args);
}

void log_debug(logger* logger, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logger_log_impl(logger, LL_DEBUG, fmt, args);
    va_end(args);
}

void log_trace(logger* logger, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logger_log_impl(logger, LL_TRACE, fmt, args);
    va_end(args);
}
