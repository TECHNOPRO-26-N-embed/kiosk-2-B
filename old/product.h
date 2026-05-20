#ifndef PRODUCT_H
#define PRODUCT_H

#define MAX_PRODUCTS 256
#define MAX_CART_ITEMS 128
#define MAX_NAME_LEN 64
#define MAX_CATEGORY_LEN 32
#define MAX_LINE_LEN 512

typedef enum {
    TAX_IN_STORE = 1,
    TAX_TAKEOUT = 2,
    TAX_FREE = 3
} TaxMode;

typedef enum {
    PAY_CASH = 1,
    PAY_CARD = 2,
    PAY_EMONEY = 3
} PaymentMethod;

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char category[MAX_CATEGORY_LEN];
    int price_excl_tax;
    int is_active;
} Product;

typedef struct {
    Product product;
    int quantity;
} CartItem;

typedef struct {
    int subtotal_excl_tax;
    int tax_amount;
    int total_incl_tax;
} TotalResult;

#endif