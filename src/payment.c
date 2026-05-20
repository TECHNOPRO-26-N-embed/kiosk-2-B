#include <stdio.h>

#include "io_utils.h"
#include "product.h"

/* 支払方法の列挙値をログ/表示向け文字列へ変換する。 */
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
 * 支払い処理本体。
 * 現金は不足分を再入力させ、カード/電子マネーは即時成功として扱う。
 */
PaymentResult payment(int total_incl_tax, PaymentMethod payment_method, int *change_amount) {
    /* 出力先ポインタ未指定は呼び出し側の不正として失敗。 */
    if (change_amount == NULL) {
        return PAYMENT_ERROR;
    }

    /* 呼び出しごとにお釣りを初期化する。 */
    *change_amount = 0;
    if (total_incl_tax < 0) {
        return PAYMENT_ERROR;
    }

    /* 非現金は金額入力不要のため即時成功扱い。 */
    if (payment_method == PAYMENT_CARD || payment_method == PAYMENT_EMONEY) {
        printf("決済完了: %s\n", payment_method_name(payment_method));
        return PAYMENT_OK;
    }

    if (payment_method == PAYMENT_CASH) {
        int paid = 0;
        int inserted = 0;

        /* 請求額に達するまで入金を繰り返す。 */
        while (paid < total_incl_tax) {
            int amount = 0;
            printf("請求金額: %d円, 入金累計: %d円\n", total_incl_tax, paid);
            if (!prompt_int("入金額を入力 (初回のみ 0 でキャンセル): ", &amount)) {
                return PAYMENT_ERROR;
            }

            /* 1円も投入していない段階だけ会計画面へ戻れる。 */
            if (amount == 0 && !inserted) {
                return PAYMENT_BACK;
            }
            if (amount <= 0) {
                printf("1以上の金額を入力してください。\n");
                continue;
            }

            inserted = 1;
            paid += amount;

            /* 未達なら不足額を提示して追加入金へ。 */
            if (paid < total_incl_tax) {
                printf("不足額: %d円\n", total_incl_tax - paid);
            }
        }

        /* 超過分をお釣りとして返す。 */
        *change_amount = paid - total_incl_tax;
        printf("お支払い完了。お釣り: %d円\n", *change_amount);
        return PAYMENT_OK;
    }

    return PAYMENT_ERROR;
}
