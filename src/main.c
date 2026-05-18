#include <stdio.h>
#include <string.h>
int main(void){
    int choice;
/*メニュー*/
    while(1){
        printf("\n=================\n");
        printf("セルフレジメニュー\n");
        printf("=================\n");
        printf("1. 商品を選択してください\n");
        printf("2. 購入を完了\n");
        printf("選択してください: ");
        scanf("%d", &choice);

        if(choice == 2){
            printf("購入を完了します。\n");
            return 0;
        }

        if(choice != 1){
            printf("無効な選択です。\n");
            continue;
        }

        while(1){
            printf("\n=================\n");
            printf("商品選択メニュー\n");
            printf("=================\n");
            printf("1. 商品A\n");
            printf("2. 商品B\n");
            printf("3. 商品C\n");
            printf("4. 戻る\n");
            printf("選択してください: ");
            scanf("%d", &choice);

            switch(choice){
                case 1:
                    printf("商品Aを選択しました。\n");
                    break;
                case 2:
                    printf("商品Bを選択しました。\n");
                    break;
                case 3:
                    printf("商品Cを選択しました。\n");
                    break;
                case 4:
                    break;
                default:
                    printf("無効な選択です。\n");
                    break;
            }

            if(choice == 4){
                break;
            }
        }
    }

    return 0;
}
/*合計金額を受け取って支払いを完了*/
int main()
{
int total;
int money = 0;
scanf("%d", &total);

printf("合計金額:%d円\n", total);

while(money < total)
{
 int pay;

 printf("お金を入力:");
 scanf("%d", &pay);

 money += pay;
}

printf("支払完了\n");
printf("お釣り:%d円\n", money - total);

printf("メインメニューへ戻る\n");

 return 0;
}