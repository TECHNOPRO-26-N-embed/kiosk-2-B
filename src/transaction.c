#include <stdio.h>
#include <string.h>
#include <time.h>
#include "transaction.h"
#include "tax.h"

void get_current_timestamp(char *buf, int buf_size) {
    time_t now  = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", t);
}

void generate_transaction_id(char *buf, int buf_size) {
    time_t now  = time(NULL);
    struct tm *t = localtime(&now);

    char date_str[9]; /* YYYYMMDD */
    strftime(date_str, sizeof(date_str), "%Y%m%d", t);

    /* 当日分の取引件数をカウントして連番を決定 */
    int seq = 1;
    FILE *fp = fopen(PATH_TRANSACTIONS, "r");
    if (fp) {
        char line[512];
        char prefix[12];
        int  prefix_len;
        snprintf(prefix, sizeof(prefix), "TX%s", date_str);
        prefix_len = (int)strlen(prefix);
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, prefix, prefix_len) == 0) {
                seq++;
            }
        }
        fclose(fp);
    }

    snprintf(buf, buf_size, "TX%s%03d", date_str, seq);
}

int write_csv_header_if_new(const char *file_path, const char *header_line) {
    /* ファイルが既に存在するなら何もしない */
    FILE *test = fopen(file_path, "r");
    if (test) { fclose(test); return 0; }

    FILE *fp = fopen(file_path, "w");
    if (!fp) {
        fprintf(stderr, "[エラー] CSVヘッダの書き込みに失敗: %s\n", file_path);
        return -1;
    }
    fprintf(fp, "%s\n", header_line);
    fclose(fp);
    return 0;
}

int save_transaction_header(const TransactionHeader *header) {
    const char *hdr =
        "transaction_id,timestamp,tax_mode,payment_method,"
        "subtotal_excl_tax,tax_amount,total_incl_tax,paid_amount,change_amount";
    if (write_csv_header_if_new(PATH_TRANSACTIONS, hdr) != 0) return -1;

    FILE *fp = fopen(PATH_TRANSACTIONS, "a");
    if (!fp) {
        fprintf(stderr, "[エラー] transactions.csv を開けません\n");
        return -1;
    }

    const char *tax_str =
        (header->tax_mode == TAX_IN_STORE) ? "IN_STORE" :
        (header->tax_mode == TAX_TAKEOUT)  ? "TAKEOUT"  : "TAX_FREE";
    const char *pay_str =
        (header->payment_method == PAY_CASH)    ? "CASH"    :
        (header->payment_method == PAY_CARD)    ? "CARD"    : "E_MONEY";

    fprintf(fp, "%s,%s,%s,%s,%d,%d,%d,%d,%d\n",
            header->transaction_id,
            header->timestamp,
            tax_str, pay_str,
            header->subtotal_excl_tax,
            header->tax_amount,
            header->total_incl_tax,
            header->paid_amount,
            header->change_amount);
    fclose(fp);
    return 0;
}

int save_transaction_items(const TransactionHeader *header,
                           const Cart *cart, TaxMode tax_mode) {
    const char *hdr =
        "transaction_id,line_no,product_id,product_name,"
        "unit_price_excl_tax,quantity,line_subtotal_excl_tax,"
        "line_tax_amount,line_total_incl_tax";
    if (write_csv_header_if_new(PATH_TRANS_ITEMS, hdr) != 0) return -1;

    FILE *fp = fopen(PATH_TRANS_ITEMS, "a");
    if (!fp) {
        fprintf(stderr, "[エラー] transaction_items.csv を開けません\n");
        return -1;
    }

    int tax_rate = get_tax_rate(tax_mode);
    int i;
    for (i = 0; i < cart->count; i++) {
        const CartItem *item = &cart->items[i];
        int line_tax   = calc_line_tax(item->line_subtotal_excl_tax, tax_rate);
        int line_total = item->line_subtotal_excl_tax + line_tax;

        fprintf(fp, "%s,%d,%d,%s,%d,%d,%d,%d,%d\n",
                header->transaction_id,
                i + 1,
                item->product_id,
                item->product_name,
                item->unit_price_excl_tax,
                item->quantity,
                item->line_subtotal_excl_tax,
                line_tax,
                line_total);
    }
    fclose(fp);
    return 0;
}

int save_transaction(const TransactionHeader *header,
                     const Cart *cart, TaxMode tax_mode) {
    int result = 0;
    if (save_transaction_header(header) != 0) result = -1;
    if (save_transaction_items(header, cart, tax_mode) != 0) result = -1;
    return result;
}
