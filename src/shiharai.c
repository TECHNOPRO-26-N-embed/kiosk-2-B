// 支払い処理
#include <stdio.h>
#include "product.h"
// 支払い処理
// 合計金額を表示し、ユーザーから支払い金額を受け付ける
// 支払いが足りない場合は繰り返し入力を促し、お釣りも計算して表示
void payment(float total) {
    int due = (int)(total + 0.5f); // 四捨五入
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