#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "types.h"
#include "cart.h"

/* 現在時刻を "YYYY-MM-DD HH:MM:SS" 形式で buf に書き込む。 */
void get_current_timestamp(char *buf, int buf_size);

/* 取引IDを生成して buf に書き込む。
   形式: TX + YYYYMMDD + 3桁連番（例: TX20260519001）。
   連番は当日分の transactions.csv の行数から自動計算する。 */
void generate_transaction_id(char *buf, int buf_size);

/* CSVファイルが存在しない場合のみヘッダ行を書き込む。
   戻り値: 0=成功、-1=ファイルオープンエラー */
int write_csv_header_if_new(const char *file_path, const char *header_line);

/* transactions.csv に取引ヘッダを1行追記する。
   戻り値: 0=成功、-1=ファイルエラー */
int save_transaction_header(const TransactionHeader *header);

/* transaction_items.csv にカートの全明細を追記する。
   行単位で税額を計算して書き込む。
   戻り値: 0=成功、-1=ファイルエラー */
int save_transaction_items(const TransactionHeader *header,
                           const Cart *cart, TaxMode tax_mode);

/* save_transaction_header と save_transaction_items をまとめて呼ぶラッパー。
   戻り値: 0=全成功、-1=いずれかでエラー */
int save_transaction(const TransactionHeader *header,
                     const Cart *cart, TaxMode tax_mode);

#endif /* TRANSACTION_H */
