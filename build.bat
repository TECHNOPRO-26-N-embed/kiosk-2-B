@echo off
echo.
echo === コンパイル中 ===
gcc src\main.c src\input.c src\product.c src\cart.c ^
    src\tax.c src\payment.c src\transaction.c ^
    src\history.c src\logger.c ^
    -o app.exe -Wall -Wextra
if %ERRORLEVEL% EQU 0 (
    echo.
    echo [成功] app.exe が生成されました
    echo 実行するには以下を入力してください:
    echo   .\app.exe
    echo.
) else (
    echo.
    echo [エラー] コンパイルに失敗しました
    echo 上のエラーメッセージを確認してください
    echo.
)
