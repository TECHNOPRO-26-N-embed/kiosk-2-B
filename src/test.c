#include <stdio.h>
#include <string.h>


// 商品マスタ
typedef struct {
    int  id;        // 商品番号 
    char name[20];  // 商品名
    int  price;     // 税抜単価 /
    int  tax_rate;  // 税率 8 or 10 /
} Product;

// 商品マスタ配列
Product master[3] = {
    {1, "商品A", 100, 8},
    {2, "商品B", 200, 8},
    {3, "商品C", 300, 10}
};

// カート（上で作ったやつをそのまま使う）
typedef struct {
    char name[20];
    int  price;     /* 税抜単価 */
    int  tax_rate;  /* 税率 */
    int  qty;       /* 数量 */
} CartItem;

CartItem cart[10];
int cart_count = 0;

// カートに追加
void add_to_cart(Product p) {
    int i;
    for (i = 0; i < cart_count; i++) {
        if (cart[i].price == p.price &&
            strcmp(cart[i].name, p.name) == 0) {
            cart[i].qty++;
            return;
        }
    }
    strcpy(cart[cart_count].name, p.name);
    cart[cart_count].price    = p.price;
    cart[cart_count].tax_rate = p.tax_rate;
    cart[cart_count].qty      = 1;
    cart_count++;
}

// カート表示
// 税込計算はintで
// 税込単価 = 税抜価格 * (100 + 税率) / 100
void show_cart(void) {
    int i;
    printf("\n===== カート =====\n");
    for (i = 0; i < cart_count; i++) {
        int taxed = cart[i].price * (100 + cart[i].tax_rate) / 100;
        printf("%s  税込%d円  x%d  小計%d円\n",
            cart[i].name,
            taxed,
            cart[i].qty,
            taxed * cart[i].qty);
    }
    printf("==================\n");
}

// メイン関数
// 各自コードのforループ部分をここに統合

int main(void) {
    int id;
    int found;

    while (1) {
        printf("\n商品番号を入力（0で会計）: ");
        scanf("%d", &id);

        if (id == 0) break;

        // forループをそのまま流用 
        found = 0;
        for (int i = 0; i < 3; i++) {
            if (id == master[i].id) {
                add_to_cart(master[i]); /* ここだけ変更 / 只改这里 */
                show_cart();
                found = 1;
                break;
            }
        }

        if (!found) {
            printf("該当する商品がありません。\n");
        }
    }

    printf("購入を完了します。\n");
    return 0;
}