#ifndef HISTORY_H
#define HISTORY_H

/* transactions.csv を末尾から最大 max_records 件読み込んで一覧表示する。
   ファイルが存在しない場合は「履歴なし」を表示する。
   不正な行はスキップして正常行のみ表示する。
   戻り値: 0=成功、-1=ファイルなし */
int show_transaction_history(int max_records);

/* transaction_items.csv から指定 transaction_id の明細行を表示する。
   戻り値: 表示した行数（0=対象なし、-1=ファイルエラー） */
int show_transaction_detail(const char *transaction_id);

#endif /* HISTORY_H */
