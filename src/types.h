#ifndef TYPES_H
#define TYPES_H

/* ========== 上限定数 ========== */
#define MAX_PRODUCTS       100
#define MAX_CART_ITEMS      50
#define MAX_PRODUCT_NAME    64
#define MAX_CATEGORY_LEN    32
#define MAX_CATEGORIES      20
#define TRANSACTION_ID_LEN  16   /* "TX20260519001\0" */
#define TIMESTAMP_LEN       20   /* "YYYY-MM-DD HH:MM:SS\0" */

/* ========== CSVファイルパス ========== */
#define PATH_PRODUCTS       "data/products.csv"
#define PATH_TRANSACTIONS   "data/transactions.csv"
#define PATH_TRANS_ITEMS    "data/transaction_items.csv"
#define PATH_OPERATION_LOG  "data/operation_log.csv"

/* ========== 税率（整数パーセント） ========== */
#define TAX_RATE_IN_STORE  10
#define TAX_RATE_TAKEOUT    8
#define TAX_RATE_FREE       0

/* ========== 列挙型 ========== */

/* 税区分 */
typedef enum {
    TAX_IN_STORE,   /* 店内（10%） */
    TAX_TAKEOUT,    /* 持ち帰り（8%） */
    TAX_FREE        /* 免税（0%） */
} TaxMode;

/* 支払方法 */
typedef enum {
    PAY_CASH,       /* 現金 */
    PAY_CARD,       /* カード */
    PAY_E_MONEY     /* 電子マネー */
} PaymentMethod;

/* ========== 構造体 ========== */

/* 商品マスタの1件 */
typedef struct {
    int  product_id;
    char product_name[MAX_PRODUCT_NAME];
    char category[MAX_CATEGORY_LEN];
    int  price_excl_tax;    /* 税抜単価（円） */
    int  is_active;         /* 1=有効, 0=無効 */
} Product;

/* カート内の1明細 */
typedef struct {
    int  product_id;
    char product_name[MAX_PRODUCT_NAME];
    int  unit_price_excl_tax;       /* 単価（税抜、円） */
    int  quantity;                  /* 数量 */
    int  line_subtotal_excl_tax;    /* 行小計（税抜 = 単価 × 数量） */
} CartItem;

/* カート全体 */
typedef struct {
    CartItem items[MAX_CART_ITEMS];
    int      count;
} Cart;

/* 商品マスタ全体 */
typedef struct {
    Product items[MAX_PRODUCTS];
    int     count;
} ProductMaster;

/* 取引ヘッダ（1回の会計単位） */
typedef struct {
    char          transaction_id[TRANSACTION_ID_LEN];
    char          timestamp[TIMESTAMP_LEN];
    TaxMode       tax_mode;
    PaymentMethod payment_method;
    int           subtotal_excl_tax;    /* 税抜合計（円） */
    int           tax_amount;           /* 税額（円） */
    int           total_incl_tax;       /* 税込合計（円） */
    int           paid_amount;          /* 預かり金額（現金時） */
    int           change_amount;        /* お釣り（現金時） */
} TransactionHeader;

#endif /* TYPES_H */
