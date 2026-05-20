#include <stdio.h>
#include <time.h>
#include <string.h>
#include "logger.h"
#include "types.h"

/* ファイルスコープのユーティリティ */
static void logger_get_timestamp(char *buf, int buf_size) {
    time_t now  = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", t);
}

static void logger_write_header_if_new(void) {
    FILE *test = fopen(PATH_OPERATION_LOG, "r");
    if (test) { fclose(test); return; }

    FILE *fp = fopen(PATH_OPERATION_LOG, "w");
    if (!fp) return;
    fprintf(fp, "timestamp,transaction_id,action,detail\n");
    fclose(fp);
}

void log_operation(const char *action, const char *detail,
                   const char *transaction_id) {
    logger_write_header_if_new();

    FILE *fp = fopen(PATH_OPERATION_LOG, "a");
    if (!fp) {
        fprintf(stderr, "[警告] 操作ログの書き込みに失敗しました（action=%s）\n",
                action ? action : "");
        return;
    }

    char ts[TIMESTAMP_LEN];
    logger_get_timestamp(ts, sizeof(ts));

    const char *tx = (transaction_id && transaction_id[0]) ? transaction_id : "";
    fprintf(fp, "%s,%s,%s,%s\n",
            ts, tx,
            action ? action : "",
            detail ? detail : "");
    fclose(fp);
}

void cancel_purchase(Cart *cart, const char *transaction_id) {
    char detail[64];
    snprintf(detail, sizeof(detail), "items_in_cart=%d", cart->count);
    log_operation("CANCEL_PURCHASE", detail, transaction_id);
    cart->count = 0;
}
