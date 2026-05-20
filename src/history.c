#include <stdio.h>
#include <string.h>
#include "history.h"
#include "types.h"

/* 循環バッファで最新 HIST_BUF 件を保持する上限 */
#define HIST_BUF 20

int show_transaction_history(int max_records) {
    if (max_records > HIST_BUF) max_records = HIST_BUF;

    FILE *fp = fopen(PATH_TRANSACTIONS, "r");
    if (!fp) {
        printf("\n[情報] 取引履歴がありません\n");
        return -1;
    }

    /* 循環バッファで末尾 max_records 件を保持 */
    char buf[HIST_BUF][512];
    int  pos       = 0;
    int  total     = 0;
    int  is_header = 1;
    char line[512];

    while (fgets(line, sizeof(line), fp)) {
        if (is_header) { is_header = 0; continue; }
        if (strlen(line) <= 1) continue;
        strncpy(buf[pos % HIST_BUF], line, 511);
        buf[pos % HIST_BUF][511] = '\0';
        pos++;
        total++;
    }
    fclose(fp);

    if (total == 0) {
        printf("\n[情報] 取引履歴がありません\n");
        return 0;
    }

    int display_count = (total < max_records) ? total : max_records;
    int start_pos     = pos - display_count;

    printf("\n===== 取引履歴（最新 %d 件） =====\n", display_count);
    printf("%-16s  %-19s  %-8s  %-10s  %s\n",
           "取引ID", "日時", "税区分", "支払方法", "税込合計");
    printf("---------------------------------------------------------------------\n");

    int i;
    for (i = 0; i < display_count; i++) {
        char tmp[512];
        strncpy(tmp, buf[(start_pos + i) % HIST_BUF], 511);
        tmp[511] = '\0';

        /* 末尾改行を除去 */
        size_t len = strlen(tmp);
        if (len > 0 && tmp[len - 1] == '\n') tmp[len - 1] = '\0';

        /* CSV分割: transaction_id,timestamp,tax_mode,payment_method,
                    subtotal_excl_tax,tax_amount,total_incl_tax,... */
        char *fields[9];
        int   fc  = 0;
        char *tok = strtok(tmp, ",");
        while (tok && fc < 9) {
            fields[fc++] = tok;
            tok = strtok(NULL, ",");
        }

        if (fc < 7) {
            printf("  [不正行をスキップ]\n");
            continue;
        }

        const char *tax_label =
            (strcmp(fields[2], "IN_STORE") == 0) ? "店内" :
            (strcmp(fields[2], "TAKEOUT")  == 0) ? "持ち帰り" : "免税";
        const char *pay_label =
            (strcmp(fields[3], "CASH")    == 0) ? "現金" :
            (strcmp(fields[3], "CARD")    == 0) ? "カード" : "電子マネー";

        printf("%-16s  %-19s  %-8s  %-10s  %s 円\n",
               fields[0], fields[1], tax_label, pay_label, fields[6]);
    }
    printf("=====================================\n");
    return 0;
}

int show_transaction_detail(const char *transaction_id) {
    FILE *fp = fopen(PATH_TRANS_ITEMS, "r");
    if (!fp) {
        printf("[エラー] 明細ファイルを開けません: %s\n", PATH_TRANS_ITEMS);
        return -1;
    }

    printf("\n--- 取引明細: %s ---\n", transaction_id);
    printf("%-4s  %-18s  %9s  %5s  %9s\n",
           "No.", "商品名", "単価(税抜)", "数量", "税込小計");
    printf("-------------------------------------------------------------\n");

    char line[512];
    int  found     = 0;
    int  is_header = 1;

    while (fgets(line, sizeof(line), fp)) {
        if (is_header) { is_header = 0; continue; }

        char tmp[512];
        strncpy(tmp, line, 511);
        tmp[511] = '\0';

        char *fields[9];
        int   fc  = 0;
        char *tok = strtok(tmp, ",");
        while (tok && fc < 9) {
            fields[fc++] = tok;
            tok = strtok(NULL, ",");
        }

        if (fc < 9) continue;
        if (strcmp(fields[0], transaction_id) != 0) continue;

        /* 末尾改行を除去 */
        size_t len = strlen(fields[8]);
        if (len > 0 && fields[8][len - 1] == '\n') fields[8][len - 1] = '\0';

        printf("%-4s  %-18s  %7s 円  %3s 個  %7s 円\n",
               fields[1], fields[3], fields[4], fields[5], fields[8]);
        found++;
    }
    fclose(fp);

    if (!found) printf("  （明細なし）\n");
    printf("-------------------------------------------------------------\n");
    return found;
}
