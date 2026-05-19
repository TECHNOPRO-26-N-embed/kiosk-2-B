#include <string.h>
#include <stdio.h>
#include <time.h>  /* 日時取得に必要 / 获取时间用 */

/* 商品マスタ / 商品主数据 */
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

/* カート / 购物车（新規追加） */
typedef struct {
    char name[20];
    int  price;
    float tax;
    int  qty;
} CartItem;

CartItem cart[10];
int cart_count = 0;

/* カートに追加 / 添加到购物车 */
void add_to_cart(char *name, int price, float tax) {
    int i;
    /* 既にあれば数量+1 / 已有则数量+1 */
    for (i = 0; i < cart_count; i++) {
        if (strcmp(cart[i].name, name) == 0) {
            cart[i].qty++;
            return;
        }
    }
    /* なければ新規追加 / 没有则新增 */
    strcpy(cart[cart_count].name, name);
    cart[cart_count].price = price;
    cart[cart_count].tax   = tax;
    cart[cart_count].qty   = 1;
    cart_count++;
}

/* カート表示 / 显示购物车 */
void show_cart(void) {
    int i;
    float subtotal;
    printf("\n===== カート =====\n");
    for (i = 0; i < cart_count; i++) {
        subtotal = cart[i].price * (1 + cart[i].tax) * cart[i].qty;
        printf("%s  %d円  x%d  小計%.0f円\n",
            cart[i].name,
            cart[i].price,
            cart[i].qty,
            subtotal);
    }
    printf("==================\n");
}

/* CSV保存 / 保存到CSV（新規追加） */
void save_to_csv(float total) {
    int i;
    FILE *fp;
    time_t now;
    struct tm *t;
    char datetime[20];

    /* 現在時刻を取得 / 获取当前时间 */
    time(&now);
    t = localtime(&now);
    sprintf(datetime, "%04d-%02d-%02d %02d:%02d:%02d",
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec);

    /* ファイルを追記モードで開く / 以追加模式打开文件 */
    fp = fopen("sales_log.csv", "a");
    if (fp == NULL) {
        printf("CSVファイルを開けませんでした\n");
        return;
    }

    /* 1商品1行で書き込む / 每个商品写一行 */
    for (i = 0; i < cart_count; i++) {
        float subtotal = cart[i].price * (1 + cart[i].tax) * cart[i].qty;
        fprintf(fp, "%s,%s,%d,%.0f,%d,%.0f\n",
            datetime,
            cart[i].name,
            cart[i].price,
            cart[i].tax * 100,
            cart[i].qty,
            subtotal);
    }

    fclose(fp);
    printf("CSVに保存しました\n");
}

/* select_product：カート追加処理だけ変更 */
void select_product(float *total) {
    int input_id, quantity, price;
    float tax;

    printf("商品リスト:\n");
    for (int i = 0; i < 3; i++) {
        printf("%d. %s - %d円 (税率: %.0f%%)\n",
            products[i].id, products[i].name,
            products[i].price, products[i].tax * 100);
    }
    printf("商品番号を入力（1～3）: ");
    scanf("%d", &input_id);

    for (int i = 0; i < 3; i++) {
        if (products[i].id == input_id) {
            price = products[i].price;
            tax   = products[i].tax;
            printf("購入個数: ");
            scanf("%d", &quantity);

            /* カートに追加 / 添加到购物车 */
            add_to_cart(products[i].name, price, tax);
            if (quantity > 1) {
                int j;
                for (j = 1; j < quantity; j++)
                    add_to_cart(products[i].name, price, tax);
            }

            *total += price * (1 + tax) * quantity;
            show_cart();
            return;
        }
    }
    printf("該当商品なし\n");
}

/* payment：最後にCSV保存を追加 */
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

    /* 支払い完了後にCSV保存 / 支付完成后保存CSV */
    save_to_csv(total);
}

int main() {
    float total = 0.0f;
    while (1) {
        int menu;
        printf("\n1. 商品選択 2. 支払い\n選択: ");
        scanf("%d", &menu);
        if (menu == 1) {
            select_product(&total);
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