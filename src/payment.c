#include <stdio.h>
#include "payment.h"
#include "input.h"

int calc_change(int total_incl_tax, int paid_amount) {
    return paid_amount - total_incl_tax;
}

int process_cash_payment(int total_incl_tax, int *paid_amount, int *change_amount) {
    *paid_amount   = 0;
    *change_amount = 0;

    printf("\n--- 現金支払い ---\n");
    printf("請求金額: %d 円\n", total_incl_tax);
    printf("※ まだ投入していない場合は 0 を入力するとキャンセルできます\n\n");

    while (*paid_amount < total_incl_tax) {
        int input = read_int_input("投入金額を入力 (円): ");

        /* 非数値入力 */
        if (input == -1) {
            printf("[エラー] 数値を入力してください\n");
            continue;
        }

        /* 投入前のみキャンセル可 */
        if (input == 0 && *paid_amount == 0) {
            printf("[キャンセル] 支払方法選択に戻ります\n");
            return -1;
        }

        /* 負の金額は無効 */
        if (input < 0) {
            printf("[エラー] 正の金額を入力してください\n");
            continue;
        }

        *paid_amount += input;
        printf("投入済み: %d 円", *paid_amount);

        if (*paid_amount < total_incl_tax) {
            printf("（不足: %d 円）\n", total_incl_tax - *paid_amount);
        } else {
            printf("\n");
        }
    }

    *change_amount = calc_change(total_incl_tax, *paid_amount);
    printf("お釣り: %d 円\n", *change_amount);
    return 0;
}

int process_noncash_payment(int total_incl_tax, const char *method_label) {
    printf("\n--- %s支払い ---\n", method_label);
    printf("請求金額: %d 円\n", total_incl_tax);
    printf("  1. 決済を確定する\n");
    printf("  0. キャンセル（支払方法選択へ戻る）\n");

    while (1) {
        int choice = read_int_input("選択: ");
        if (choice == 1)  return 0;
        if (choice == 0) {
            printf("[キャンセル] 支払方法選択に戻ります\n");
            return -1;
        }
        printf("[エラー] 1 または 0 を入力してください\n");
    }
}

int process_payment(int total_incl_tax, PaymentMethod method,
                    int *paid_amount, int *change_amount) {
    *paid_amount   = 0;
    *change_amount = 0;

    switch (method) {
    case PAY_CASH:
        return process_cash_payment(total_incl_tax, paid_amount, change_amount);
    case PAY_CARD:
        return process_noncash_payment(total_incl_tax, "カード");
    case PAY_E_MONEY:
        return process_noncash_payment(total_incl_tax, "電子マネー");
    default:
        return -1;
    }
}
