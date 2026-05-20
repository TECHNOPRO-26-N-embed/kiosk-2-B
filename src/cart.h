#ifndef CART_H
#define CART_H

#include "types.h"

/* カートを空にリセットする。 */
void init_cart(Cart *cart);

/* カートに商品を追加する。
   同一 product_id が既にカートにあれば数量を加算する。
   戻り値: 0=成功、-1=カート満杯 */
int  add_to_cart(Cart *cart, const Product *product, int quantity);

/* line_no（1始まり）の行をカートから削除して後続行を詰める。
   戻り値: 0=成功、-1=範囲外 */
int  delete_cart_item(Cart *cart, int line_no);

/* カート明細を番号付きで画面に表示する。
   カートが空の場合は「空」を表示する。 */
void display_cart(const Cart *cart);

/* カートが空なら 1、商品があれば 0 を返す。 */
int  is_cart_empty(const Cart *cart);

#endif /* CART_H */
