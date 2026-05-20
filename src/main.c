#include <stdio.h>

#include "io_utils.h"
#include "product.h"

/*
 * プログラム全体の入口。
 * 起動時に商品マスタを読み込み、メインメニュー（購入/履歴/終了）を繰り返し処理する。
 */
int main(void) {
    Product products[MAX_PRODUCTS];
    int product_count = -1;

    /* 起動時は商品マスタが正常に読めるまでここで待機する。 */
    while (1) {
        int retry = 0;
        product_count = load_products(PRODUCTS_FILE, products, MAX_PRODUCTS);
        if (product_count > 0) {
            /* 有効商品が1件以上あればメイン処理へ進む。 */
            printf("商品マスタ読込: %d 件\n", product_count);
            break;
        }

        if (product_count == 0) {
            printf("有効な商品が0件です。購入機能は利用できません。\n");
        } else {
            printf("商品マスタCSVの読込に失敗しました: %s\n", PRODUCTS_FILE);
        }

        if (!prompt_int("1: 再試行, 9: 終了 > ", &retry)) {
            /* 標準入力が閉じた場合は異常終了扱い。 */
            return 1;
        }
        if (retry == 9) {
            return 1;
        }
    }

    /* 常時メニューを表示し、選択に応じて各機能を呼び出す。 */
    while (1) {
        int menu = 0;
        printf("\n=== メインメニュー ===\n");
        printf("1: 購入\n");
        printf("2: 取引履歴\n");
        printf("9: 終了\n");
        if (!prompt_int("選択 > ", &menu)) {
            break;
        }

        if (menu == 1) {
            if (product_count <= 0) {
                /* 商品0件の場合、購入処理には進ませない。 */
                printf("商品がないため購入できません。\n");
                continue;
            }
            run_purchase_flow(products, product_count);
        } else if (menu == 2) {
            int n = 10;
            /* 0入力は既定値10件に置き換える。 */
            if (prompt_int("表示件数（最新N件, 0で既定10） > ", &n) && n == 0) {
                n = 10;
            }
            show_history(n);
        } else if (menu == 9) {
            printf("終了します。\n");
            break;
        } else {
            printf("無効な選択です。\n");
        }
    }

    return 0;
}
