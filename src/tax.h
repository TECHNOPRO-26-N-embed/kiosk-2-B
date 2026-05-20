#ifndef TAX_H
#define TAX_H

#include "types.h"

/* TaxMode に対応する税率を整数パーセントで返す。
   例: TAX_IN_STORE → 10、TAX_TAKEOUT → 8、TAX_FREE → 0 */
int get_tax_rate(TaxMode tax_mode);

/* 税抜小計に税率を掛けた税額を四捨五入で返す（整数演算）。
   計算式: (小計 × 税率 + 50) / 100
   例: calc_line_tax(240, 10) → 24 */
int calc_line_tax(int line_subtotal_excl_tax, int tax_rate_percent);

/* カート全体の税抜合計・税額・税込合計を計算する（行単位で四捨五入）。
   戻り値: 0=成功、-1=カートが空またはNULL */
int calc_total(const Cart *cart, TaxMode tax_mode,
               int *out_subtotal_excl, int *out_tax, int *out_total_incl);

#endif /* TAX_H */
