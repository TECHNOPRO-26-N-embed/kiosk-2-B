#ifndef PRODUCT_H
#define PRODUCT_H

#define MAX_PRODUCTS 200
#define MAX_CART_ITEMS 100
#define MAX_NAME_LEN 64
#define MAX_CATEGORY_LEN 32
#define MAX_LINE_LEN 512

#define PRODUCTS_FILE "data/products.csv"
#define TRANSACTIONS_FILE "data/transactions.csv"
#define TRANSACTION_ITEMS_FILE "data/transaction_items.csv"
#define OPERATION_LOG_FILE "data/operation_log.csv"

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char category[MAX_CATEGORY_LEN];
    int price_excl_tax;
    double product_tax_rate;
    int is_active;
} Product;

typedef struct {
    int product_id;
    char product_name[MAX_NAME_LEN];
    int unit_price;
    int quantity;
} CartItem;

typedef enum {
    TAX_IN_STORE = 1,
    TAX_TAKEOUT = 2
} TaxMode;

typedef enum {
    PAYMENT_CASH = 1,
    PAYMENT_CARD = 2,
    PAYMENT_EMONEY = 3
} PaymentMethod;

typedef struct {
    int subtotal_excl_tax;
    int tax_amount;
    int total_incl_tax;
} TotalResult;

typedef enum {
    PAYMENT_OK = 0,
    PAYMENT_BACK = 1,
    PAYMENT_ERROR = 2
} PaymentResult;

typedef enum {
    SELECT_PRODUCT_ABORT = -1,
    SELECT_PRODUCT_BACK = 0,
    SELECT_PRODUCT_ADDED = 1,
    SELECT_PRODUCT_CHECKOUT = 2
} SelectProductResult;

int load_products(const char *file_path, Product products[], int max_products);
SelectProductResult select_product(const Product products[],
                                   int product_count,
                                   const char *category,
                                   CartItem cart_items[],
                                   int *cart_count);
int calc_total(const CartItem cart_items[], int cart_count, TaxMode tax_mode, TotalResult *out_total);
PaymentResult payment(int total_incl_tax, PaymentMethod payment_method, int *change_amount);
int save_transaction(const CartItem cart_items[], int cart_count, TaxMode tax_mode, PaymentMethod payment_method, const TotalResult *total, char *out_transaction_id, int out_size);
void show_history(int latest_count);
void log_operation(const char *action, const char *detail, const char *transaction_id);
void cancel_purchase(CartItem cart_items[], int *cart_count, int items_in_cart_count);
int delete_cart_item(CartItem cart_items[], int *cart_count, int line_no);
void run_purchase_flow(Product products[], int product_count);

#endif