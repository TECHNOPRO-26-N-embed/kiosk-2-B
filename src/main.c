<<<<<<< Updated upstream
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
=======
﻿#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "types.h"
#include "input.h"
#include "product.h"
#include "cart.h"
#include "tax.h"
#include "payment.h"
#include "transaction.h"
#include "history.h"
#include "logger.h"

typedef enum {
    PS_SELECT_CATEGORY,
    PS_SELECT_PRODUCT,
    PS_INPUT_QUANTITY,
    PS_CONFIRM_CART,
    PS_SELECT_TAX,
    PS_SELECT_PAYMENT_METHOD,
    PS_PROCESS_PAYMENT,
    PS_COMPLETE,
    PS_CANCELLED
} PurchaseState;

/* メインメニューを表示して入力値(1/2/9)を返す */
int show_main_menu(void) {
    printf("\n");
    printf("=====================================\n");
    printf("       セルフレジ ver1.1\n");
    printf("=====================================\n");
    printf("  1. 購入開始\n");
    printf("  2. 取引履歴表示\n");
    printf("  9. 終了\n");
    printf("-------------------------------------\n");
    return read_int_input("選択してください: ");
}

/* カテゴリ選択画面。selected_category に選択結果を格納する。
   戻り値: 1=選択完了、0=キャンセル */
int screen_select_category(const ProductMaster *master, char *selected_category) {
    char categories[MAX_CATEGORIES][MAX_CATEGORY_LEN];
    while (1) {
        int cat_count = collect_categories(master, categories, MAX_CATEGORIES);
        if (cat_count == 0) {
            printf("[エラー] 有効なカテゴリがありません\n");
            return 0;
        }
        display_category_list(categories, cat_count);
        int choice = read_int_input("カテゴリ番号を入力: ");
        if (choice == 0) return 0;
        if (choice < 1 || choice > cat_count) {
            printf("[エラー] 1〜%d の番号を入力してください\n", cat_count);
            continue;
        }
        strncpy(selected_category, categories[choice - 1], MAX_CATEGORY_LEN - 1);
        selected_category[MAX_CATEGORY_LEN - 1] = '\0';
        char detail[64];
        snprintf(detail, sizeof(detail), "category=%s", selected_category);
        log_operation("SELECT_CATEGORY", detail, "");
        return 1;
    }
}

/* 商品選択画面。*selected に選択した商品へのポインタを格納する。
   戻り値: 1=選択完了、0=キャンセル（カテゴリ選択へ戻る） */
int screen_select_product(const ProductMaster *master, const char *category,
                          const Product **selected) {
    while (1) {
        display_products_in_category(master, category);
        int product_id = read_int_input("商品IDを入力: ");
        if (product_id == 0) return 0;
        if (product_id == -1) {
            printf("[エラー] 数値を入力してください\n");
            continue;
        }
        const Product *p = find_product_by_id(master, product_id);
        if (!p) {
            printf("[エラー] 商品ID %d は存在しないか無効です\n", product_id);
            continue;
        }
        if (strcmp(p->category, category) != 0) {
            printf("[エラー] 商品ID %d は選択したカテゴリにありません\n", product_id);
            continue;
        }
        *selected = p;
        return 1;
    }
}

/* 数量入力画面。*out_quantity に入力値を格納する。
   戻り値: 1=確定、0=キャンセル（商品選択へ戻る） */
int screen_input_quantity(const Product *product, int *out_quantity) {
    printf("\n--- 数量入力 ---\n");
    printf("商品: %s（%d 円/個）\n", product->product_name, product->price_excl_tax);
    while (1) {
        int qty = read_int_input("数量を入力（0でキャンセル）: ");
        if (qty == 0) return 0;
        if (qty == -1) { printf("[エラー] 数値を入力してください\n"); continue; }
        if (qty < 0)   { printf("[エラー] 1 以上の整数を入力してください\n"); continue; }
        *out_quantity = qty;
        return 1;
    }
}

/* カート確認画面。削除操作は内部ループで処理する。
   戻り値: 1=会計へ進む、2=商品追加を続ける、0=購入キャンセル */
int screen_confirm_cart(Cart *cart) {
    while (1) {
        display_cart(cart);
        printf("  1. 会計へ進む\n");
        printf("  2. 商品を追加する\n");
        if (!is_cart_empty(cart)) {
            printf("  3. カート内の商品を削除する\n");
        }
        printf("  0. 購入キャンセル（メインメニューへ）\n");
        printf("------------------------------\n");
        int choice = read_int_input("選択: ");
        switch (choice) {
        case 1:
            if (is_cart_empty(cart)) {
                printf("[エラー] カートが空です。商品を追加してください\n");
                continue;
>>>>>>> Stashed changes
            }
            return 1;
        case 2:
            return 2;
        case 3:
            if (!is_cart_empty(cart)) {
                int line = read_int_input("削除する行番号を入力: ");
                if (line == -1 || delete_cart_item(cart, line) != 0) {
                    printf("[エラー] 無効な行番号です\n");
                } else {
                    char detail[32];
                    snprintf(detail, sizeof(detail), "line_no=%d", line);
                    log_operation("DELETE_ITEM", detail, "");
                    printf("[完了] 削除しました\n");
                }
            } else {
                printf("[エラー] カートが空です\n");
            }
            continue;
        case 0:
            return 0;
        default:
            printf("[エラー] 有効な選択肢を入力してください\n");
            continue;
        }
    }
}

/* 税区分選択画面。*out_mode に選択結果を格納する。
   戻り値: 1=選択完了、0=キャンセル（カート確認へ戻る） */
int screen_select_tax(TaxMode *out_mode) {
    printf("\n--- 税区分選択 ---\n");
    printf("  1. 店内（税率 10%%）\n");
    printf("  2. 持ち帰り（税率 8%%）\n");
    printf("  3. 免税（税率 0%%）\n");
    printf("  0. キャンセル（カート確認へ戻る）\n");
    printf("-------------------\n");
    while (1) {
        int choice = read_int_input("選択: ");
        switch (choice) {
        case 1: *out_mode = TAX_IN_STORE; return 1;
        case 2: *out_mode = TAX_TAKEOUT;  return 1;
        case 3: *out_mode = TAX_FREE;     return 1;
        case 0: return 0;
        default: printf("[エラー] 1〜3 または 0 を入力してください\n");
        }
    }
}

/* 支払方法選択画面。*out_method に選択結果を格納する。
   戻り値: 1=選択完了、0=キャンセル（税区分選択へ戻る） */
int screen_select_payment_method(PaymentMethod *out_method) {
    printf("\n--- 支払方法選択 ---\n");
    printf("  1. 現金\n");
    printf("  2. カード\n");
    printf("  3. 電子マネー\n");
    printf("  0. キャンセル（税区分選択へ戻る）\n");
    printf("--------------------\n");
    while (1) {
        int choice = read_int_input("選択: ");
        switch (choice) {
        case 1: *out_method = PAY_CASH;    return 1;
        case 2: *out_method = PAY_CARD;    return 1;
        case 3: *out_method = PAY_E_MONEY; return 1;
        case 0: return 0;
        default: printf("[エラー] 1〜3 または 0 を入力してください\n");
        }
    }
}

/* 完了画面を表示する（取引ID・合計・お釣りなどを表示）。 */
void screen_show_completion(const TransactionHeader *header) {
    const char *tax_label =
        (header->tax_mode == TAX_IN_STORE) ? "店内（10%）" :
        (header->tax_mode == TAX_TAKEOUT)  ? "持ち帰り（8%）" : "免税（0%）";
    const char *pay_label =
        (header->payment_method == PAY_CASH)    ? "現金" :
        (header->payment_method == PAY_CARD)    ? "カード" : "電子マネー";
    printf("\n");
    printf("========================================\n");
    printf("    お買い上げありがとうございます\n");
    printf("========================================\n");
    printf("  取引ID  : %s\n", header->transaction_id);
    printf("  日時    : %s\n", header->timestamp);
    printf("  税区分  : %s\n", tax_label);
    printf("  支払方法: %s\n", pay_label);
    printf("  -------\n");
    printf("  税抜合計: %6d 円\n", header->subtotal_excl_tax);
    printf("  消費税  : %6d 円\n", header->tax_amount);
    printf("  税込合計: %6d 円\n", header->total_incl_tax);
    if (header->payment_method == PAY_CASH) {
        printf("  お預かり: %6d 円\n", header->paid_amount);
        printf("  お釣り  : %6d 円\n", header->change_amount);
    }
    printf("========================================\n");
    printf("\nエンターキーを押すとメインメニューへ戻ります...");
    fflush(stdout);
    clear_input_buffer();
}

/* 購入フロー全体を状態機械で制御する。
   カテゴリ選択 -> 商品選択 -> 数量入力 -> カート確認
   -> 税区分選択 -> 支払方法選択 -> 支払い処理 -> 完了 */
void run_purchase_flow(ProductMaster *master) {
    Cart          cart;
    PurchaseState state = PS_SELECT_CATEGORY;
    char          selected_category[MAX_CATEGORY_LEN] = "";
    const Product *selected_product = NULL;
    int           quantity    = 0;
    TaxMode       tax_mode    = TAX_IN_STORE;
    PaymentMethod pay_method  = PAY_CASH;
    int           subtotal_excl = 0, tax_amt = 0, total_incl = 0;
    int           paid = 0, change = 0;
    TransactionHeader header;

    init_cart(&cart);

    while (state != PS_COMPLETE && state != PS_CANCELLED) {
        switch (state) {
        case PS_SELECT_CATEGORY:
            if (!screen_select_category(master, selected_category))
                state = PS_CANCELLED;
            else
                state = PS_SELECT_PRODUCT;
            break;

        case PS_SELECT_PRODUCT:
            selected_product = NULL;
            if (!screen_select_product(master, selected_category, &selected_product))
                state = PS_SELECT_CATEGORY;
            else
                state = PS_INPUT_QUANTITY;
            break;

        case PS_INPUT_QUANTITY:
            quantity = 0;
            if (!screen_input_quantity(selected_product, &quantity)) {
                state = PS_SELECT_PRODUCT;
            } else {
                if (add_to_cart(&cart, selected_product, quantity) == 0) {
                    char detail[128];
                    snprintf(detail, sizeof(detail), "product_id=%d quantity=%d",
                             selected_product->product_id, quantity);
                    log_operation("ADD_ITEM", detail, "");
                    printf("[追加] %s を %d 個カートに追加しました\n",
                           selected_product->product_name, quantity);
                }
                state = PS_CONFIRM_CART;
            }
            break;

        case PS_CONFIRM_CART: {
            int r = screen_confirm_cart(&cart);
            if (r == 0)      state = PS_CANCELLED;
            else if (r == 2) state = PS_SELECT_CATEGORY;
            else             state = PS_SELECT_TAX;
            break;
        }

        case PS_SELECT_TAX:
            if (!screen_select_tax(&tax_mode)) {
                state = PS_CONFIRM_CART;
            } else {
                calc_total(&cart, tax_mode, &subtotal_excl, &tax_amt, &total_incl);
                printf("\n--- 金額確認 ---\n");
                printf("  税抜合計: %d 円\n", subtotal_excl);
                printf("  消費税  : %d 円\n", tax_amt);
                printf("  税込合計: %d 円\n", total_incl);
                state = PS_SELECT_PAYMENT_METHOD;
            }
            break;

        case PS_SELECT_PAYMENT_METHOD:
            if (!screen_select_payment_method(&pay_method))
                state = PS_SELECT_TAX;
            else
                state = PS_PROCESS_PAYMENT;
            break;

        case PS_PROCESS_PAYMENT:
            paid = 0; change = 0;
            if (process_payment(total_incl, pay_method, &paid, &change) != 0) {
                state = PS_SELECT_PAYMENT_METHOD;
            } else {
                generate_transaction_id(header.transaction_id, sizeof(header.transaction_id));
                get_current_timestamp(header.timestamp, sizeof(header.timestamp));
                header.tax_mode          = tax_mode;
                header.payment_method    = pay_method;
                header.subtotal_excl_tax = subtotal_excl;
                header.tax_amount        = tax_amt;
                header.total_incl_tax    = total_incl;
                header.paid_amount       = paid;
                header.change_amount     = change;

                if (save_transaction(&header, &cart, tax_mode) != 0)
                    printf("[警告] 取引データの保存に失敗しました\n");

                char log_detail[128];
                snprintf(log_detail, sizeof(log_detail), "method=%s total=%d",
                         pay_method == PAY_CASH ? "CASH" :
                         pay_method == PAY_CARD ? "CARD" : "E_MONEY",
                         total_incl);
                log_operation("PAYMENT_COMPLETED", log_detail, header.transaction_id);
                state = PS_COMPLETE;
            }
            break;

        default:
            state = PS_CANCELLED;
            break;
        }
    }

    if (state == PS_COMPLETE) {
        screen_show_completion(&header);
    } else {
        cancel_purchase(&cart, "");
        printf("[情報] 購入をキャンセルしてメインメニューへ戻ります\n");
    }
}

/* 取引履歴フロー（メニュー選択 2）を実行する。 */
void run_history_flow(void) {
    show_transaction_history(10);
    while (1) {
        printf("\n  d. 取引明細を表示する\n");
        printf("  0. メインメニューへ戻る\n");
        char input[TRANSACTION_ID_LEN + 4];
        read_string_input("選択: ", input, sizeof(input));
        if (strcmp(input, "0") == 0) return;
        if (strcmp(input, "d") == 0) {
            char tx_id[TRANSACTION_ID_LEN];
            read_string_input("取引IDを入力: ", tx_id, sizeof(tx_id));
            show_transaction_detail(tx_id);
        } else {
            printf("[エラー] 0 または d を入力してください\n");
        }
    }
}

int main(void) {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    printf("セルフレジ ver1.1 を起動しています...\n");

    ProductMaster master;
    int load_result = load_products(PATH_PRODUCTS, &master);

    if (load_result < 0) {
        printf("[エラー] 商品マスタの読み込みに失敗しました\n");
        printf("  1. 再試行する\n");
        printf("  9. 終了する\n");
        int choice = read_int_input("選択: ");
        if (choice == 1) {
            load_result = load_products(PATH_PRODUCTS, &master);
            if (load_result < 0) {
                printf("[エラー] 再試行に失敗しました。終了します\n");
                return 1;
            }
        } else {
            return 0;
        }
    }

    if (master.count == 0) {
        printf("[警告] 有効な商品がありません。data/products.csv を確認してください\n");
    } else {
        printf("[完了] 商品マスタを読み込みました（%d 件）\n", master.count);
    }

    while (1) {
        int choice = show_main_menu();
        switch (choice) {
        case 1:
            if (master.count == 0)
                printf("[エラー] 商品マスタが空のため購入できません\n");
            else
                run_purchase_flow(&master);
            break;
        case 2:
            run_history_flow();
            break;
        case 9:
            printf("\nご利用ありがとうございました。終了します\n");
            return 0;
        default:
            printf("[エラー] 1、2、または 9 を入力してください\n");
            break;
        }
    }

    return 0;
}
<<<<<<< Updated upstream


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
=======
>>>>>>> Stashed changes
