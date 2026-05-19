// 商品情報構造体と関数プロトタイプ宣言
#ifndef PRODUCT_H
#define PRODUCT_H

typedef struct {
    int id;         // 商品番号
    char name[20];  // 商品名
    int price;      // 価格（税抜）
    float tax;      // 税率
} Product;

extern Product products[];
void select_product(int *id, int *quantity, int *price, float *tax);
void payment(float total);

#endif