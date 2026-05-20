#include <stdio.h>
#include <string.h>
#include "cart.h"

void init_cart(Cart *cart) {
    cart->count = 0;
}

int add_to_cart(Cart *cart, const Product *product, int quantity) {
    /* 同一商品IDが既にカートにあれば数量を加算 */
    int i;
    for (i = 0; i < cart->count; i++) {
        if (cart->items[i].product_id == product->product_id) {
            cart->items[i].quantity += quantity;
            cart->items[i].line_subtotal_excl_tax =
                cart->items[i].unit_price_excl_tax * cart->items[i].quantity;
            return 0;
        }
    }

    if (cart->count >= MAX_CART_ITEMS) {
        printf("[エラー] カートが満杯です（上限 %d 件）\n", MAX_CART_ITEMS);
        return -1;
    }

    CartItem *item              = &cart->items[cart->count];
    item->product_id            = product->product_id;
    strncpy(item->product_name, product->product_name, MAX_PRODUCT_NAME - 1);
    item->product_name[MAX_PRODUCT_NAME - 1] = '\0';
    item->unit_price_excl_tax   = product->price_excl_tax;
    item->quantity              = quantity;
    item->line_subtotal_excl_tax = product->price_excl_tax * quantity;

    cart->count++;
    return 0;
}

int delete_cart_item(Cart *cart, int line_no) {
    if (line_no < 1 || line_no > cart->count) {
        return -1;
    }
    int i;
    int idx = line_no - 1;
    for (i = idx; i < cart->count - 1; i++) {
        cart->items[i] = cart->items[i + 1];
    }
    cart->count--;
    return 0;
}

void display_cart(const Cart *cart) {
    printf("\n===== カート確認 =====\n");
    if (cart->count == 0) {
        printf("  （カートは空です）\n");
        printf("======================\n");
        return;
    }
    printf("%-4s  %-18s  %9s  %5s  %10s\n",
           "No.", "商品名", "単価(税抜)", "数量", "小計(税抜)");
    printf("--------------------------------------------------------------\n");
    int i;
    for (i = 0; i < cart->count; i++) {
        const CartItem *it = &cart->items[i];
        printf("%-4d  %-18s  %7d 円  %3d 個  %8d 円\n",
               i + 1,
               it->product_name,
               it->unit_price_excl_tax,
               it->quantity,
               it->line_subtotal_excl_tax);
    }
    printf("======================\n");
}

int is_cart_empty(const Cart *cart) {
    return cart->count == 0;
}
