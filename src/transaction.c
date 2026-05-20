#include <stdio.h>
#include <string.h>
#include <time.h>

#include "product.h"

/* ファイルが空（未作成含む）かどうかを判定する。 */
static int file_is_empty(const char *path) {
    FILE *fp;
    int c;

    /* 読み込みで開けない場合は未作成とみなし空と判定する。 */
    fp = fopen(path, "r");
    if (fp == NULL) {
        return 1;
    }

    c = fgetc(fp);
    fclose(fp);
    return (c == EOF);
}

/* 現在時刻を YYYYMMDDhhmmss 形式で文字列化する。 */
static void now_timestamp(char *out, int size) {
    time_t now;
    struct tm *tm_now;

    if (out == NULL || size <= 0) {
        return;
    }

    /* 現在時刻を取得し、CSVで扱いやすい固定フォーマットへ整形する。 */
    now = time(NULL);
    tm_now = localtime(&now);
    if (tm_now == NULL) {
        strncpy(out, "19700101000000", size - 1);
        out[size - 1] = '\0';
        return;
    }

    strftime(out, size, "%Y%m%d%H%M%S", tm_now);
}

/* 税区分の列挙値をCSV出力向け文字列へ変換する。 */
static const char *tax_mode_name(TaxMode mode) {
    if (mode == TAX_IN_STORE) {
        return "IN_STORE";
    }
    if (mode == TAX_TAKEOUT) {
        return "TAKEOUT";
    }
    return "UNKNOWN";
}

/* 支払方法の列挙値をCSV出力向け文字列へ変換する。 */
static const char *payment_method_name(PaymentMethod method) {
    if (method == PAYMENT_CASH) {
        return "CASH";
    }
    if (method == PAYMENT_CARD) {
        return "CARD";
    }
    if (method == PAYMENT_EMONEY) {
        return "EMONEY";
    }
    return "UNKNOWN";
}

/*
 * 取引ヘッダと取引明細をCSVへ追記保存する。
 * 保存成功時は 1、失敗時は 0 を返す。
 */
int save_transaction(const CartItem cart_items[], int cart_count, TaxMode tax_mode, PaymentMethod payment_method, const TotalResult *total, char *out_transaction_id, int out_size) {
    FILE *fp_header;
    FILE *fp_items;
    int i;
    char timestamp[32];
    static int seq = 0;

    /* 引数不正なら保存処理を行わない。 */
    if (cart_count <= 0 || total == NULL || out_transaction_id == NULL || out_size <= 0) {
        return 0;
    }

    /* 取引IDは timestamp + 連番 で一意になるよう作成する。 */
    now_timestamp(timestamp, sizeof(timestamp));
    seq++;
    snprintf(out_transaction_id, out_size, "%s-%03d", timestamp, seq);

    /* 取引ヘッダCSVへ追記する。 */
    fp_header = fopen(TRANSACTIONS_FILE, "a");
    if (fp_header == NULL) {
        printf("取引ヘッダ保存に失敗しました。\n");
        return 0;
    }

    /* 初回作成時のみヘッダ行を先に出力する。 */
    if (file_is_empty(TRANSACTIONS_FILE)) {
        fprintf(fp_header, "transaction_id,timestamp,payment_method,tax_mode,subtotal_excl_tax,tax_amount,total_incl_tax,item_count\n");
    }

    fprintf(fp_header, "%s,%s,%s,%s,%d,%d,%d,%d\n",
            out_transaction_id,
            timestamp,
            payment_method_name(payment_method),
            tax_mode_name(tax_mode),
            total->subtotal_excl_tax,
            total->tax_amount,
            total->total_incl_tax,
            cart_count);
    fclose(fp_header);

    /* 取引明細CSVへ追記する。 */
    fp_items = fopen(TRANSACTION_ITEMS_FILE, "a");
    if (fp_items == NULL) {
        printf("取引明細保存に失敗しました。\n");
        return 0;
    }

    if (file_is_empty(TRANSACTION_ITEMS_FILE)) {
        fprintf(fp_items, "transaction_id,line_no,product_id,product_name,unit_price,quantity,line_subtotal\n");
    }

    /* カートの各明細を1行ずつ保存する。 */
    for (i = 0; i < cart_count; i++) {
        int line_subtotal = cart_items[i].unit_price * cart_items[i].quantity;
        fprintf(fp_items, "%s,%d,%d,%s,%d,%d,%d\n",
                out_transaction_id,
                i + 1,
                cart_items[i].product_id,
                cart_items[i].product_name,
                cart_items[i].unit_price,
                cart_items[i].quantity,
                line_subtotal);
    }
    fclose(fp_items);
    return 1;
}

/* transactions.csv から最新N件の履歴を読み込み表示する。 */
void show_history(int latest_count) {
    FILE *fp = fopen(TRANSACTIONS_FILE, "r");
    char lines[1000][MAX_LINE_LEN];
    int count = 0;
    int start;
    int i;

    /* latest_count が不正な場合は既定値に補正する。 */
    if (latest_count <= 0) {
        latest_count = 10;
    }

    if (fp == NULL) {
        printf("履歴なし（transactions.csv が見つかりません）\n");
        return;
    }

    /* 先頭ヘッダ込みで最大1000行まで読み込む。 */
    while (count < 1000 && fgets(lines[count], sizeof(lines[count]), fp) != NULL) {
        int len = (int)strlen(lines[count]);
        while (len > 0 && (lines[count][len - 1] == '\n' || lines[count][len - 1] == '\r')) {
            lines[count][len - 1] = '\0';
            len--;
        }
        count++;
    }
    fclose(fp);

    if (count <= 1) {
        printf("履歴なし\n");
        return;
    }

    /* 先頭行はヘッダのため、表示開始位置は最低でも1。 */
    start = count - latest_count;
    if (start < 1) {
        start = 1;
    }

    printf("\n---- 取引履歴（最新%d件） ----\n", latest_count);
    printf("%s\n", lines[0]);
    for (i = start; i < count; i++) {
        printf("%s\n", lines[i]);
    }
    printf("-----------------------------\n");
}

/* 操作ログを operation_log.csv へ1行追記する。 */
void log_operation(const char *action, const char *detail, const char *transaction_id) {
    FILE *fp;
    char timestamp[32];

    if (action == NULL) {
        return;
    }

    /* ログ追記は本処理の補助扱い。失敗時は警告のみで戻る。 */
    fp = fopen(OPERATION_LOG_FILE, "a");
    if (fp == NULL) {
        fprintf(stderr, "[WARN] operation_log.csv write failed\n");
        return;
    }

    if (file_is_empty(OPERATION_LOG_FILE)) {
        fprintf(fp, "timestamp,transaction_id,action,detail\n");
    }

    now_timestamp(timestamp, sizeof(timestamp));
    fprintf(fp,
            "%s,%s,%s,%s\n",
            timestamp,
            (transaction_id != NULL) ? transaction_id : "",
            action,
            (detail != NULL) ? detail : "");

    fclose(fp);
}

/* 購入キャンセル時にカートを初期化し、キャンセル操作をログに残す。 */
void cancel_purchase(CartItem cart_items[], int *cart_count, int items_in_cart_count) {
    char detail[64];
    int i;

    if (cart_count == NULL) {
        return;
    }

    /* 既存カートを全消去し、件数を0に戻す。 */
    for (i = 0; i < *cart_count; i++) {
        memset(&cart_items[i], 0, sizeof(CartItem));
    }
    *cart_count = 0;

    snprintf(detail, sizeof(detail), "items_in_cart=%d", items_in_cart_count);
    log_operation("CANCEL_PURCHASE", detail, "");
    printf("購入をキャンセルしました。メインメニューへ戻ります。\n");
}

/* 指定行のカート明細を削除し、後続データを前詰めする。 */
int delete_cart_item(CartItem cart_items[], int *cart_count, int line_no) {
    int i;

    if (cart_count == NULL || *cart_count <= 0) {
        printf("カートに商品がありません。\n");
        return 0;
    }

    if (line_no < 1 || line_no > *cart_count) {
        printf("無効な番号です。\n");
        return 0;
    }

    /* 削除位置より後ろの要素を1つ前へ詰める。 */
    for (i = line_no - 1; i < *cart_count - 1; i++) {
        cart_items[i] = cart_items[i + 1];
    }
    memset(&cart_items[*cart_count - 1], 0, sizeof(CartItem));
    (*cart_count)--;

    printf("カート %d 行目を削除しました。\n", line_no);
    return 1;
}
