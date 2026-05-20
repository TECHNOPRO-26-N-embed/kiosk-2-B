#include <stdio.h>
#include <string.h>

#include "io_utils.h"
#include "product.h"

/*
 * 商品マスタCSVを読み込み、有効商品のみ products 配列へ展開する。
 * 正常時は読込件数、失敗時は -1 を返す。
 */
int load_products(const char *file_path, Product products[], int max_products) {
    FILE *fp = fopen(file_path, "r");
    char line[MAX_LINE_LEN];
    int line_no = 0;
    int count = 0;

    /* ファイルが開けない場合は呼び出し側へ失敗を返す。 */
    if (fp == NULL) {
        return -1;
    }

    /* 1行ずつCSVを解釈し、妥当なデータだけ配列へ積む。 */
    while (fgets(line, sizeof(line), fp) != NULL) {
        char *tokens[8];
        int token_count = 0;
        int id = 0;
        int price = 0;
        int  is_active= 1;
        double product_tax_rate = 0.0;
        char category[MAX_CATEGORY_LEN] = "食品";
        char *tok;

        line_no++;
        trim_newline(line);
        /* 空行は無視して次の行へ進む。 */
        if (line[0] == '\0') {
            continue;
        }

        /* カンマ区切りで分割し、最大8列まで取得する。 */
        tok = strtok(line, ",");
        while (tok != NULL && token_count < 8) {
            while (*tok == ' ' || *tok == '\t') {
                tok++;
            }
            tokens[token_count++] = tok;
            tok = strtok(NULL, ",");
        }

        /* 最低限 id,name,price がない行は不正扱い。 */
        if (token_count < 3) {
            fprintf(stderr, "[WARN] products.csv line %d skipped (too few columns)\n", line_no);
            continue;
        }

        /* 先頭列が数値でない場合はヘッダまたは不正行としてスキップ。 */
        if (!parse_int_strict(tokens[0], &id)) {
            if (line_no == 1) {
                continue;
            }
            fprintf(stderr, "[WARN] products.csv line %d skipped (invalid id)\n", line_no);
            continue;
        }

        /* 5列以上かつ3列目が数値でない場合はカテゴリ列あり形式として解釈する。 */
        if (token_count >= 5 && !parse_int_strict(tokens[2], &price)) {
            strncpy(category, tokens[2], sizeof(category) - 1);
            category[sizeof(category) - 1] = '\0';

            /* 4列目は価格。数値変換できない場合は不正行。 */
            if (!parse_int_strict(tokens[3], &price)) {
                fprintf(stderr, "[WARN] products.csv line %d skipped (invalid price)\n", line_no);
                continue;
            }

            if (token_count >= 6) {
                /* 5列目は税率 or is_active を許容する。 */
                if (!parse_double_strict(tokens[4], &product_tax_rate)) {
                    if (!parse_int_strict(tokens[4], &is_active)) {
                        fprintf(stderr, "[WARN] products.csv line %d skipped (invalid tax/is_active)\n", line_no);
                        continue;
                    }
                }
                /* 6列目があれば is_active として解釈する。 */
                if (token_count >= 7 && !parse_int_strict(tokens[5], &is_active)) {
                    fprintf(stderr, "[WARN] products.csv line %d skipped (invalid is_active)\n", line_no);
                    continue;
                }
            } else {
                /* 5列目しかない場合は is_active として解釈する。 */
                if (!parse_int_strict(tokens[4], &is_active)) {
                    fprintf(stderr, "[WARN] products.csv line %d skipped (invalid is_active)\n", line_no);
                    continue;
                }
            }
        } else {
            /* 3列目を価格として解釈する通常形式。 */
            if (!parse_int_strict(tokens[2], &price)) {
                fprintf(stderr, "[WARN] products.csv line %d skipped (invalid price)\n", line_no);
                continue;
            }

            if (token_count >= 4) {
                double parsed_tax = 0.0;
                int parsed_active = 0;
                /* 4列目は税率または is_active として柔軟に解釈。 */
                if (parse_double_strict(tokens[3], &parsed_tax)) {
                    product_tax_rate = parsed_tax;
                    if (token_count >= 5 && !parse_int_strict(tokens[4], &is_active)) {
                        fprintf(stderr, "[WARN] products.csv line %d skipped (invalid is_active)\n", line_no);
                        continue;
                    }
                } else if (parse_int_strict(tokens[3], &parsed_active)) {
                    is_active = parsed_active;
                } else {
                    fprintf(stderr, "[WARN] products.csv line %d skipped (invalid tax/is_active)\n", line_no);
                    continue;
                }
            }
        }

        /* 非アクティブ商品は選択対象から除外する。 */
        if (!is_active) {
            continue;
        }

        /* 配列上限に達した場合はそれ以上取り込まない。 */
        if (count >= max_products) {
            fprintf(stderr, "[WARN] products.csv has more than %d active items; excess ignored\n", max_products);
            break;
        }

        /* 妥当な行だけをProduct構造体へ格納する。 */
        products[count].id = id;
        strncpy(products[count].name, tokens[1], sizeof(products[count].name) - 1);
        products[count].name[sizeof(products[count].name) - 1] = '\0';
        strncpy(products[count].category, category, sizeof(products[count].category) - 1);
        products[count].category[sizeof(products[count].category) - 1] = '\0';
        products[count].price_excl_tax = price;
        products[count].product_tax_rate = product_tax_rate;
        products[count].is_active = 1;
        count++;
    }

    /* ファイルを閉じ、最終的な取込件数を返す。 */
    fclose(fp);
    return count;
}
