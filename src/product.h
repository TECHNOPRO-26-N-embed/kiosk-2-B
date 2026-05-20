#ifndef PRODUCT_H
#define PRODUCT_H

#include "types.h"

/* products.csv を読み込んでマスタ配列を構築する。
   ヘッダ行は自動スキップ。列数不足・数値変換失敗の行はスキップして警告を表示する。
   戻り値: 読み込んだ有効件数（ファイルオープン失敗時 -1） */
int load_products(const char *file_path, ProductMaster *master);

/* マスタから is_active=1 の商品のカテゴリを重複なしで収集する。
   戻り値: 収集したカテゴリ数 */
int collect_categories(const ProductMaster *master,
                       char categories[][MAX_CATEGORY_LEN], int max_categories);

/* カテゴリ一覧を番号付きで画面に表示する。末尾に「0: キャンセル」を表示する。 */
void display_category_list(char categories[][MAX_CATEGORY_LEN], int count);

/* 指定カテゴリの有効商品（is_active=1）を商品ID・商品名・単価の形式で表示する。 */
void display_products_in_category(const ProductMaster *master, const char *category);

/* 商品IDでマスタを線形検索する（is_active=1 の商品のみ対象）。
   戻り値: 見つかった Product へのポインタ（存在しない場合 NULL） */
const Product *find_product_by_id(const ProductMaster *master, int product_id);

#endif /* PRODUCT_H */
