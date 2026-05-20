#ifndef LOGGER_H
#define LOGGER_H

#include "types.h"
#include "cart.h"

/* operation_log.csv に1行追記する。
   action 例: SELECT_CATEGORY / ADD_ITEM / DELETE_ITEM /
              PAYMENT_COMPLETED / CANCEL_PURCHASE
   transaction_id が未発行の場合は "" を渡す。
   ファイル書き込み失敗時は stderr へ出力し、本処理は継続する。 */
void log_operation(const char *action, const char *detail,
                   const char *transaction_id);

/* カートをクリアし、CANCEL_PURCHASE アクションをログに記録する。
   transaction_id が未発行の場合は "" を渡す。 */
void cancel_purchase(Cart *cart, const char *transaction_id);

#endif /* LOGGER_H */
