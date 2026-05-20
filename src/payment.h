#ifndef PAYMENT_H
#define PAYMENT_H

#include "types.h"

/* 支払い処理のエントリポイント。method に応じた関数を呼び出す。
   paid_amount / change_amount は現金時のみ設定され、カード/電子マネー時は 0 になる。
   戻り値: 0=決済完了、-1=キャンセル（支払方法選択へ戻る） */
int process_payment(int total_incl_tax, PaymentMethod method,
                    int *paid_amount, int *change_amount);

/* 現金支払い処理。請求額に達するまで入金を繰り返す。
   投入額が 0 かつ paid_amount=0 の場合はキャンセルとする。
   戻り値: 0=完了、-1=キャンセル */
int process_cash_payment(int total_incl_tax, int *paid_amount, int *change_amount);

/* カード/電子マネー支払い処理。確認画面を表示して確定/キャンセルを受け付ける。
   戻り値: 0=完了、-1=キャンセル */
int process_noncash_payment(int total_incl_tax, const char *method_label);

/* 税込合計と預かり金額からお釣りを計算して返す。 */
int calc_change(int total_incl_tax, int paid_amount);

#endif /* PAYMENT_H */
