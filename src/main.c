#include <stdio.h>

// 商品情報
typedef struct {
    int id;
    char name[20];
    int price;
    float tax;
} Product;

Product products[] = {
    {1, "商品A", 100, 0.08f},
    {2, "商品B", 200, 0.08f},
    {3, "商品C", 300, 0.10f}
};

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

float calc_total(int price, float tax, int quantity) {
    return price * (1 + tax) * quantity;
}

void payment(float total) {
    int due = (int)(total + 0.5f);
    int paid = 0;
    int input;

    printf("合計: %d円\n", due);
    while (paid < due) {
        printf("支払い金額（残り%d円）: ", due - paid);
        scanf("%d", &input);
        if (input <= 0) {
            printf("1円以上を入力してください\n");
            continue;
        }
        paid += input;
        if (paid < due) printf("不足しています\n");
    }
    printf("お釣り: %d円\n", paid - due);
}

int main() {
    int id = 0, quantity = 0, price = 0;
    float tax = 0.0f, total = 0.0f;
    while (1) {
        int menu;
        printf("\n1. 商品選択 2. 支払い\n選択: ");
        scanf("%d", &menu);
        if (menu == 1) {
            select_product(&id, &quantity, &price, &tax);
            if (id) total += calc_total(price, tax, quantity);
            printf("合計: %.2f円\n", total);
        } else if (menu == 2) {
            if (total > 0) {
                payment(total);
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