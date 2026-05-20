#include "tax.h"

int get_tax_rate(TaxMode tax_mode) {
    switch (tax_mode) {
    case TAX_IN_STORE: return TAX_RATE_IN_STORE;
    case TAX_TAKEOUT:  return TAX_RATE_TAKEOUT;
    case TAX_FREE:     return TAX_RATE_FREE;
    default:           return TAX_RATE_IN_STORE;
    }
}

int calc_line_tax(int line_subtotal_excl_tax, int tax_rate_percent) {
    /* 四捨五入: (小計 × 税率 + 50) / 100 */
    return (line_subtotal_excl_tax * tax_rate_percent + 50) / 100;
}

int calc_total(const Cart *cart, TaxMode tax_mode,
               int *out_subtotal_excl, int *out_tax, int *out_total_incl) {
    if (!cart || cart->count == 0) return -1;

    int tax_rate = get_tax_rate(tax_mode);
    int subtotal = 0;
    int tax_sum  = 0;
    int i;

    for (i = 0; i < cart->count; i++) {
        int line_sub = cart->items[i].line_subtotal_excl_tax;
        int line_tax = calc_line_tax(line_sub, tax_rate);
        subtotal    += line_sub;
        tax_sum     += line_tax;
    }

    *out_subtotal_excl = subtotal;
    *out_tax           = tax_sum;
    *out_total_incl    = subtotal + tax_sum;
    return 0;
}
