#include "product.h"
#include <stddef.h>

/* 税区分に対応する税率を返す。未定義の税区分は -1.0。 */
static double tax_rate_for_mode(TaxMode mode) {
    if (mode == TAX_IN_STORE) {
        return 0.10;
    }
    if (mode == TAX_TAKEOUT) {
        return 0.08;
    }
    return -1.0;
}

/*
 * カート明細から税抜小計・税額・税込合計を計算する。
 * 入力不正（空カート/税区分不正/NULL）の場合は -1 を返す。
 */
int calc_total(const CartItem cart_items[], int cart_count, TaxMode tax_mode, TotalResult *out_total) {
    int subtotal = 0;
    int i;
    double rate;

    /* 出力先未指定、または空カートは計算不可。 */
    if (cart_count <= 0 || out_total == NULL) {
        return -1;
    }

    /* 税区分が定義外ならエラーで返す。 */
    rate = tax_rate_for_mode(tax_mode);
    if (rate < 0.0) {
        return -1;
    }

    /* 各明細の 単価 x 数量 を税抜小計へ加算する。 */
    for (i = 0; i < cart_count; i++) {
        subtotal += cart_items[i].unit_price * cart_items[i].quantity;
    }

    /* 税抜小計から税額を計算し、最終的な税込合計を作る。 */
    out_total->subtotal_excl_tax = subtotal;
    out_total->tax_amount = (int)(subtotal * rate);
    out_total->total_incl_tax = out_total->subtotal_excl_tax + out_total->tax_amount;
    return 0;
}
