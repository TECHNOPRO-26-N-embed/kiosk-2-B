#ifndef INPUT_H
#define INPUT_H

/* プロンプトを表示して整数を1つ読む。
   非数値入力時はバッファをクリアして -1 を返す。
   0入力は 0 として返す（キャンセル判定は呼び出し側で行う）。 */
int  read_int_input(const char *prompt);

/* プロンプトを表示して文字列を読む（末尾の改行は除去済み）。 */
void read_string_input(const char *prompt, char *buf, int max_len);

/* scanf後に残った入力バッファを空にする。 */
void clear_input_buffer(void);

#endif /* INPUT_H */
