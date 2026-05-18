#include <stdio.h>
#include <string.h>
/*int main(void){
    int choice;

    while(1){
        printf("\n=================\n");
        printf("セルフレジメニュー\n");
        printf("=================\n");
        printf("1. 商品を選択してください\n");
        printf("2. 購入を完了\n");
        printf("選択してください: ");
        scanf("%d", &choice);

        if(choice == 2){
            printf("購入を完了します。\n");
            return 0;
        }

        if(choice != 1){
            printf("無効な選択です。\n");
            continue;
        }

        while(1){
            printf("\n=================\n");
            printf("商品選択メニュー\n");
            printf("=================\n");
            printf("1. 商品A\n");
            printf("2. 商品B\n");
            printf("3. 商品C\n");
            printf("4. 戻る\n");
            printf("選択してください: ");
            scanf("%d", &choice);

            switch(choice){
                case 1:
                    printf("商品Aを選択しました。\n");
                    break;
                case 2:
                    printf("商品Bを選択しました。\n");
                    break;
                case 3:
                    printf("商品Cを選択しました。\n");
                    break;
                case 4:
                    break;
                default:
                    printf("無効な選択です。\n");
                    break;
            }

            if(choice == 4){
                break;
            }
        }
    }

    return 0;
*/

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
/*
typedef struct {
char name[32];  商品名 
int price;     単価 
int qty;       数量 
} CartItem;

CartItem cart[10];
int cart_count = 0;

/* カートに追加 */
void add_to_cart(char *name, int price) {
int i;

for (i = 0; i < cart_count; i++) {
    if (strcmp(cart[i].name, name) == 0) {
    cart[i].qty++;
return;
}
}
strcpy(cart[cart_count].name, name);
cart[cart_count].price = price;
cart[cart_count].qty = 1;
cart_count++;
}

void show_cart(void) {
int i;
printf("\n===== カート =====\n");
for (i = 0; i < cart_count; i++) {
printf("%s  %d円  x%d\n",
cart[i].name,cart[i].price,cart[i].qty);
}
printf("==================\n");
}
*/