

#include <stdio.h>
#include "product.h"



// 合計金額（税込）を計算する関数
float calc_total(int price, float tax, int quantity) {
    return price * (1 + tax) * quantity;
}


// メイン関数
// メニューを表示し、商品選択や支払い処理を繰り返す
int main() {
    int id = 0, quantity = 0, price = 0;
    float tax = 0.0f, total = 0.0f;
    while (1) {
        int menu;
        printf("\n1. 商品選択 2. 支払い\n選択: ");
        scanf("%d", &menu);
        if (menu == 1) {
            select_product(&id, &quantity, &price, &tax); // 商品選択
            if (id) total += calc_total(price, tax, quantity); // 合計金額に加算
            printf("合計: %.2f円\n", total);
        } else if (menu == 2) {
            if (total > 0) {
                payment(total); // 支払い処理
                break;
            } else {
                printf("商品未選択\n");
            }
        } else {
            printf("無効な選択\n");
        }
    }
    return 0;
}