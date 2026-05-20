#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "product.h"

int load_products(const char *file_path, ProductMaster *master) {
    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        fprintf(stderr, "[エラー] 商品マスタファイルを開けません: %s\n", file_path);
        return -1;
    }

    master->count = 0;
    char line[256];
    int line_no = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_no++;

        /* ヘッダ行をスキップ */
        if (line_no == 1) continue;

        /* 末尾の改行を除去 */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strlen(line) == 0) continue;

        if (master->count >= MAX_PRODUCTS) {
            fprintf(stderr, "[警告] 商品マスタが上限（%d 件）に達しました\n", MAX_PRODUCTS);
            break;
        }

        /* CSV分割: product_id,product_name,category,price_excl_tax,is_active */
        char tmp[256];
        strncpy(tmp, line, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = '\0';

        char *fields[5];
        int   field_count = 0;
        char *tok = strtok(tmp, ",");
        while (tok && field_count < 5) {
            fields[field_count++] = tok;
            tok = strtok(NULL, ",");
        }

        if (field_count < 5) {
            fprintf(stderr, "[警告] 行 %d は列数不足のためスキップします\n", line_no);
            continue;
        }

        Product *p        = &master->items[master->count];
        p->product_id     = atoi(fields[0]);
        strncpy(p->product_name, fields[1], MAX_PRODUCT_NAME - 1);
        p->product_name[MAX_PRODUCT_NAME - 1] = '\0';
        strncpy(p->category, fields[2], MAX_CATEGORY_LEN - 1);
        p->category[MAX_CATEGORY_LEN - 1] = '\0';
        p->price_excl_tax = atoi(fields[3]);
        p->is_active      = atoi(fields[4]);

        if (p->product_id <= 0 || p->price_excl_tax < 0) {
            fprintf(stderr, "[警告] 行 %d は不正な数値のためスキップします\n", line_no);
            continue;
        }

        master->count++;
    }

    fclose(fp);
    return master->count;
}

int collect_categories(const ProductMaster *master,
                       char categories[][MAX_CATEGORY_LEN], int max_categories) {
    int count = 0;
    int i, j;

    for (i = 0; i < master->count; i++) {
        if (!master->items[i].is_active) continue;

        /* 重複チェック */
        int already = 0;
        for (j = 0; j < count; j++) {
            if (strcmp(categories[j], master->items[i].category) == 0) {
                already = 1;
                break;
            }
        }
        if (!already && count < max_categories) {
            strncpy(categories[count], master->items[i].category, MAX_CATEGORY_LEN - 1);
            categories[count][MAX_CATEGORY_LEN - 1] = '\0';
            count++;
        }
    }
    return count;
}

void display_category_list(char categories[][MAX_CATEGORY_LEN], int count) {
    int i;
    printf("\n--- カテゴリ選択 ---\n");
    for (i = 0; i < count; i++) {
        printf("  %d. %s\n", i + 1, categories[i]);
    }
    printf("  0. キャンセル（メインメニューへ）\n");
    printf("--------------------\n");
}

void display_products_in_category(const ProductMaster *master, const char *category) {
    printf("\n--- 商品一覧 [%s] ---\n", category);
    printf("  %-6s  %-20s  %s\n", "商品ID", "商品名", "単価(税抜)");
    printf("  ----------------------------------------\n");

    int found = 0;
    int i;
    for (i = 0; i < master->count; i++) {
        const Product *p = &master->items[i];
        if (!p->is_active) continue;
        if (strcmp(p->category, category) != 0) continue;
        printf("  %-6d  %-20s  %d 円\n",
               p->product_id, p->product_name, p->price_excl_tax);
        found++;
    }

    if (!found) {
        printf("  （該当商品なし）\n");
    }
    printf("  0. キャンセル（カテゴリ選択へ）\n");
    printf("  ----------------------------------------\n");
}

const Product *find_product_by_id(const ProductMaster *master, int product_id) {
    int i;
    for (i = 0; i < master->count; i++) {
        if (master->items[i].product_id == product_id &&
            master->items[i].is_active) {
            return &master->items[i];
        }
    }
    return NULL;
}
