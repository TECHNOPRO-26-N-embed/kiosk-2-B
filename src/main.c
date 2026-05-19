#include <string.h>
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


/*
typedef struct {
    char name[32]; // 商品名 
    int  price;    // 単価
    int  qty;      // 数量
} CartItem;

CartItem cart[10];
int cart_count = 0;

// カートに追加
void add_to_cart(char *name, int price) {
    int i;
    // 既にあれば数量+1
    for (i = 0; i < cart_count; i++) {
        if (strcmp(cart[i].name, name) == 0) {
            cart[i].qty++;
            return;
        }
    }
    // なければ新規追加
    strcpy(cart[cart_count].name, name);
    cart[cart_count].price = price;
    cart[cart_count].qty   = 1;
    cart_count++;
}

// カート表示 
void show_cart(void) {
    int i;
    printf("\n===== カート =====\n");
    for (i = 0; i < cart_count; i++) {
        printf("%s  %d円  x%d\n",
            cart[i].name,
            cart[i].price,
            cart[i].qty);
    }
    printf("==================\n");
}

int main(void)
{
    int id;
    int quantity;
    int found = 0;
    int item_id[3] = {1, 2, 3};
    char item_name[3][20] = {"商品A", "商品B", "商品C"};
    int item_price[3] = {100, 200, 300};
    float tax[3] = {0.08f, 0.08f, 0.10f};

    printf("商品番号: ");
    scanf("%d", &id);

    for (int i = 0; i < 3; i++) {
        if (id == item_id[i]) {
            printf("購入個数: ");
            scanf("%d", &quantity);
            printf("商品名: %s\n", item_name[i]);
            printf("価格: %d円\n", item_price[i]);
            printf("税込単価: %.2f円\n", item_price[i] * (1 + tax[i]));
            printf("税込合計: %.2f円\n", item_price[i] * (1 + tax[i]) * quantity);
            printf("購入リストに追加\n");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("該当する商品がありません。\n");
    }

    return 0;
}
*/