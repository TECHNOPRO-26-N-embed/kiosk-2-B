#include <stdio.h>
#include <string.h>

#include "io_utils.h"
#include "product.h"

/* 商品IDで商品を検索し、見つかった要素へのポインタを返す。 */
static Product *find_product_by_id(Product products[], int count, int product_id) {
    int i;
    /* 先頭から順に走査し、一致する有効商品を返す。 */
    for (i = 0; i < count; i++) {
        if (products[i].id == product_id && products[i].is_active) {
            return &products[i];
        }
    }
    return NULL;
}

/* 商品配列から重複なしカテゴリ一覧を作成する。 */
static int collect_categories(const Product products[], int product_count, char categories[][MAX_CATEGORY_LEN], int max_categories) {
    int i;
    int count = 0;

    /* 商品からカテゴリを抽出し、重複しない一覧を作る。 */
    for (i = 0; i < product_count; i++) {
        int exists = 0;
        int j;

        for (j = 0; j < count; j++) {
            if (strcmp(categories[j], products[i].category) == 0) {
                exists = 1;
                break;
            }
        }

        /* 既出でなければ一覧へ追加する。 */
        if (!exists && count < max_categories) {
            strncpy(categories[count], products[i].category, MAX_CATEGORY_LEN - 1);
            categories[count][MAX_CATEGORY_LEN - 1] = '\0';
            count++;
        }
    }

    /* カテゴリを1件も作れない場合は既定カテゴリを入れる。 */
    if (count == 0 && max_categories > 0) {
        strncpy(categories[0], "食品", MAX_CATEGORY_LEN - 1);
        categories[0][MAX_CATEGORY_LEN - 1] = '\0';
        count = 1;
    }

    return count;
}

/* カートへ商品を追加する。同一商品は数量を加算する。 */
static int add_to_cart(CartItem cart_items[], int *cart_count, const Product *product, int quantity) {
    int i;
    if (cart_count == NULL || product == NULL) {
        return 0;
    }

    /* 既に同一商品がある場合は新規行を増やさず数量加算。 */
    for (i = 0; i < *cart_count; i++) {
        if (cart_items[i].product_id == product->id) {
            cart_items[i].quantity += quantity;
            return 1;
        }
    }

    if (*cart_count >= MAX_CART_ITEMS) {
        return 0;
    }

    /* 新規明細として末尾に追加する。 */
    cart_items[*cart_count].product_id = product->id;
    strncpy(cart_items[*cart_count].product_name, product->name, MAX_NAME_LEN - 1);
    cart_items[*cart_count].product_name[MAX_NAME_LEN - 1] = '\0';
    cart_items[*cart_count].unit_price = product->price_excl_tax;
    cart_items[*cart_count].quantity = quantity;
    (*cart_count)++;
    return 1;
}

/* 現在のカート内容を1行ずつ表示する。 */
static void print_cart(const CartItem cart_items[], int cart_count) {
    int i;
    int subtotal = 0;

    printf("\n---- カート ----\n");
    /* 明細表示と同時に税抜小計を再集計する。 */
    for (i = 0; i < cart_count; i++) {
        int line_subtotal = cart_items[i].unit_price * cart_items[i].quantity;
        subtotal += line_subtotal;
        printf("%d) %s (ID:%d) %d円 x %d = %d円\n",
               i + 1,
               cart_items[i].product_name,
               cart_items[i].product_id,
               cart_items[i].unit_price,
               cart_items[i].quantity,
               line_subtotal);
    }
    printf("税抜小計: %d円\n", subtotal);
    printf("--------------\n");
}

/*
 * 指定カテゴリの商品を表示し、商品選択と数量入力を受け付ける。
 * 追加成功/戻る/会計へ進む/中断 を戻り値で返す。
 */
SelectProductResult select_product(const Product products[],
                                   int product_count,
                                   const char *category,
                                   CartItem cart_items[],
                                   int *cart_count) {
    /* 商品選択画面は、戻る/会計遷移までループする。 */
    while (1) {
        int i;
        int product_id = 0;
        const Product *selected = NULL;

        /* 選択中カテゴリの商品だけ一覧表示する。 */
        printf("\n--- 商品一覧（カテゴリ: %s）---\n", category);
        for (i = 0; i < product_count; i++) {
            if (strcmp(products[i].category, category) == 0) {
                printf("%d: %s (%d円)\n", products[i].id, products[i].name, products[i].price_excl_tax);
            }
        }
        printf("0: カテゴリ選択へ戻る\n");
        printf("9: カート確認へ\n");

        /* 0:カテゴリへ戻る, 9:カート確認へ遷移 */
        if (!prompt_int("商品ID > ", &product_id)) {
            return SELECT_PRODUCT_ABORT;
        }
        if (product_id == 0) {
            return SELECT_PRODUCT_BACK;
        }
        if (product_id == 9) {
            return (*cart_count > 0) ? SELECT_PRODUCT_CHECKOUT : SELECT_PRODUCT_BACK;
        }

        /* 入力されたIDがカテゴリ内に存在するか検証する。 */
        for (i = 0; i < product_count; i++) {
            if (products[i].id == product_id && products[i].is_active && strcmp(products[i].category, category) == 0) {
                selected = &products[i];
                break;
            }
        }

        if (selected == NULL) {
            printf("該当商品なし。\n");
            continue;
        }

        /* 数量入力は正しい値が入るまで繰り返す。 */
        while (1) {
            int quantity = 0;
            char detail[128];

            if (!prompt_int("数量（1以上, 0で戻る） > ", &quantity)) {
                return SELECT_PRODUCT_ABORT;
            }
            if (quantity == 0) {
                break;
            }
            if (quantity < 1) {
                printf("1以上を入力してください。\n");
                continue;
            }

            if (!add_to_cart(cart_items, cart_count, selected, quantity)) {
                printf("カート上限に達しました。\n");
                return SELECT_PRODUCT_BACK;
            }

            /* 商品追加と同時に操作ログを残す。 */
            snprintf(detail, sizeof(detail), "product_id=%d,qty=%d", selected->id, quantity);
            log_operation("ADD_ITEM", detail, "");
            printf("追加: %s x %d\n", selected->name, quantity);
            return SELECT_PRODUCT_ADDED;
        }
    }
}

/*
 * 購入フロー全体（カテゴリ選択→商品追加→税計算→支払→保存）を実行する。
 * キャンセル時や入力中断時はメインメニューへ戻る。
 */
void run_purchase_flow(Product products[], int product_count) {
    CartItem cart_items[MAX_CART_ITEMS];
    int cart_count = 0;
    char categories[50][MAX_CATEGORY_LEN];
    int category_count = collect_categories(products, product_count, categories, 50);
    int proceed_to_checkout = 0;

    /* 購入開始時はカートとログ状態を初期化する。 */
    memset(cart_items, 0, sizeof(cart_items));
    log_operation("START_PURCHASE", "", "");

    /* 1段目: カテゴリ選択と商品追加を繰り返す。 */
    while (!proceed_to_checkout) {
        int category_choice = 0;
        int i;

        printf("\n--- カテゴリ選択（0でキャンセル） ---\n");
        for (i = 0; i < category_count; i++) {
            printf("%d: %s\n", i + 1, categories[i]);
        }
        printf("9: カート確認へ\n");

        /* カテゴリ画面で 0 は購入全体キャンセル。 */
        if (!prompt_int("選択 > ", &category_choice)) {
            return;
        }
        if (category_choice == 0) {
            cancel_purchase(cart_items, &cart_count, cart_count);
            return;
        }
        if (category_choice == 9) {
            if (cart_count <= 0) {
                printf("カートが空です。\n");
                continue;
            }
            proceed_to_checkout = 1;
            break;
        }
        if (category_choice < 1 || category_choice > category_count) {
            printf("無効な選択です。\n");
            continue;
        }

        /* カテゴリに応じて select_product を呼び、遷移結果で分岐する。 */
        while (1) {
            SelectProductResult result = select_product(products,
                                                        product_count,
                                                        categories[category_choice - 1],
                                                        cart_items,
                                                        &cart_count);
            if (result == SELECT_PRODUCT_ABORT) {
                return;
            }
            if (result == SELECT_PRODUCT_CHECKOUT) {
                proceed_to_checkout = 1;
                break;
            }
            if (result == SELECT_PRODUCT_BACK) {
                break;
            }
        }
    }

    /* 2段目: カート確認→税区分→支払→保存の会計処理。 */
    while (1) {
        char command[32];
        TotalResult total;
        TaxMode tax_mode;
        PaymentMethod payment_method;
        int change = 0;
        PaymentResult pay_result;
        char transaction_id[64];

        if (cart_count <= 0) {
            printf("カートが空です。購入フローを終了します。\n");
            return;
        }

        print_cart(cart_items, cart_count);
        printf("1: 会計へ進む  2: 商品追加へ戻る  d: 商品削除  0: 購入キャンセル\n");
        printf("選択 > ");
        /* カート画面コマンドは文字入力（1/2/d/0）で判定する。 */
        if (!read_line(command, sizeof(command))) {
            return;
        }

        if (strcmp(command, "0") == 0) {
            cancel_purchase(cart_items, &cart_count, cart_count);
            return;
        }
        if (strcmp(command, "2") == 0) {
            run_purchase_flow(products, product_count);
            return;
        }
        if (strcmp(command, "d") == 0 || strcmp(command, "D") == 0) {
            int line_no = 0;
            if (!prompt_int("削除する行番号 > ", &line_no)) {
                return;
            }
            delete_cart_item(cart_items, &cart_count, line_no);
            continue;
        }
        if (strcmp(command, "1") != 0) {
            printf("無効な選択です。\n");
            continue;
        }

        /* 税区分選択は会計へ進む前に必須。 */
        while (1) {
            int t = 0;
            printf("\n税区分を選択してください（0で戻る）\n");
            printf("1: 店内(10%%)  2: 持ち帰り(8%%)\n");
            if (!prompt_int("税区分 > ", &t)) {
                return;
            }
            if (t == 0) {
                break;
            }
            if (t < 1 || t > 2) {
                printf("無効な税区分です。\n");
                continue;
            }
            tax_mode = (TaxMode)t;

            /* 税計算に失敗した場合は継続せず終了する。 */
            if (calc_total(cart_items, cart_count, tax_mode, &total) != 0) {
                printf("金額計算エラー。\n");
                return;
            }

            printf("税抜: %d円 / 税額: %d円 / 税込合計: %d円\n",
                   total.subtotal_excl_tax,
                   total.tax_amount,
                   total.total_incl_tax);

            /* 支払方法は0で1つ前（税区分）へ戻れる。 */
            while (1) {
                int p = 0;
                printf("\n支払方法（0で税区分へ戻る）\n");
                printf("1: 現金  2: カード  3: 電子マネー\n");
                if (!prompt_int("支払方法 > ", &p)) {
                    return;
                }
                if (p == 0) {
                    break;
                }
                if (p < 1 || p > 3) {
                    printf("無効な支払方法です。\n");
                    continue;
                }

                /* 決済成功まで支払入力を継続する。 */
                payment_method = (PaymentMethod)p;
                pay_result = payment(total.total_incl_tax, payment_method, &change);
                if (pay_result == PAYMENT_BACK) {
                    continue;
                }
                if (pay_result != PAYMENT_OK) {
                    printf("決済エラー。\n");
                    continue;
                }

                /* 決済後に取引CSV保存。失敗時は安全に中断。 */
                transaction_id[0] = '\0';
                if (!save_transaction(cart_items,
                                      cart_count,
                                      tax_mode,
                                      payment_method,
                                      &total,
                                      transaction_id,
                                      sizeof(transaction_id))) {
                    log_operation("SAVE_TRANSACTION_FAIL", "save failed", "");
                    printf("保存に失敗したため処理を中断しました。\n");
                    return;
                }

                log_operation("PAYMENT_SUCCESS", "", transaction_id);
                printf("\n会計完了。取引ID: %s\n", transaction_id);
                if (payment_method == PAYMENT_CASH) {
                    printf("お釣り: %d円\n", change);
                }
                return;
            }
        }
    }
}
