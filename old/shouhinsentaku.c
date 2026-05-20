// 商品選択処理
#include <stdio.h>
#include "product.h"

// 商品リスト（グローバル変数として定義）
Product products[] = {
    {1, "商品A", 100, 0.08f},
    {2, "商品B", 200, 0.08f},
    {3, "商品C", 300, 0.10f}
};

// 商品選択処理
// 商品リストを表示し、ユーザーに商品番号と個数を入力させる

void select_product(int *id, int *quantity, int *price, float *tax) {
    int input_id;
    printf("商品リスト:\n");
    for (int i = 0; i < 3; i++) {   
        printf("%d. %s - %d円 (税率: %.0f%%)\n", products[i].id, products[i].name, products[i].price, products[i].tax * 100);
    }
    printf("商品番号を入力（1～3）: ");
    scanf("%d", &input_id);
    for (int i = 0; i < 3; i++) {
        if (products[i].id == input_id) {
            *id = input_id;
            *price = products[i].price;
            *tax = products[i].tax;
            printf("購入個数: ");
            scanf("%d", quantity);
            return;
        }
    }
    printf("該当商品なし\n");
    *id = 0;
}
