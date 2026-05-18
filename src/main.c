#include <stdio.h>
int main(void){
    int choice;
    while(1){
        printf("\n=================\n");
        printf("セルフレジメニュー\n");
        printf("=================\n");
        printf("1. 商品をスキャン\n");
        printf("2. 購入を完了\n");
        printf("選択してください: ");
        scanf("%d", &choice);
        switch(choice){
            case 1:
                printf("商品をスキャンします。\n");
                break;
            case 2:
                printf("購入を完了します。\n");
                return 0;
                
            default:
                printf("無効な選択です。\n");
        }
    }
}