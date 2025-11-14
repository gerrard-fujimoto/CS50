#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t BYTE;

// FATのブロックサイズを定義
#define BLOCK_SIZE 512
 
int main(int argc, char *argv[])
{
    // コマンドライン引数が1つではない場合
    if (argc != 2)
    {
        printf("Usage: %s ./recover image\n", argv[0]);
        return 1;
    }

    FILE *raw_file = fopen(argv[1], "r");

    // rawファイルが開けない場合
    if (raw_file == NULL)
    {
        return 1;
    }

    // 1-3バイト目のJPGヘッダ
    const uint8_t FIRST_BYTE  = 0xff;
    const uint8_t SECOND_BYTE = 0xd8;
    const uint8_t THIRD_BYTE  = 0xff;

    // 4バイト目のJPGヘッダのチェック用
    const uint8_t FOURTH_BYTE_MASK = 0xf0;
    const uint8_t JPG_MARKER_START = 0xe0; // jpgでは4バイト目がデータ種類を扱う値が入り、マーカーと呼ばれる

    // jpgはFATファイルシステムのため、512バイトの「入れ物」を用意
    BYTE buffer[BLOCK_SIZE];

    // ファイル名に必要なバイト
    const int FILE_NAME_BYTE = 8;

    int file_count = 0;
    bool is_opened = false;

    // jpgファイルを以下whileの外で利用する場合があるため、ここでFILE型を定義
    FILE *jpg_file;

    while (fread(buffer, BLOCK_SIZE, 1, raw_file))
    {
        // ファイル名の8バイトの箱を用意
        char filename[FILE_NAME_BYTE];

        // jpg判定
        if (buffer[0] == FIRST_BYTE &&
        buffer[1] == SECOND_BYTE &&
        buffer[2] == THIRD_BYTE &&
        (buffer[3] & FOURTH_BYTE_MASK) == JPG_MARKER_START) // 4つ目のバイトの値が0xe0~0xefのレンジ内であるか
        {
            // 2回目以降に新しいjpgファイルが見つかった時
            if (is_opened)
            {
                fclose(jpg_file);
                is_opened = false;
            }

            // ファイル名が###.jpgになっていない場合
            int bytes_written = snprintf(filename, FILE_NAME_BYTE, "%03i.jpg", file_count);
            if (bytes_written < 0 ||
            bytes_written >= FILE_NAME_BYTE)
            {
                fclose(raw_file);
                return 1;
            }

            // jpgファイルを書き込み権限で開いておく
            jpg_file = fopen(filename, "w");

            // 新規jpgファイルの生成が成功したら、ファイル名の数字を更新
            file_count++;

            // ファイルが開いた状態にしておく
            is_opened = true;

            // jpgファイルが開けない場合
            if (jpg_file == NULL)
            {
                fclose(raw_file);
                return 1;
            }

            fwrite(buffer, BLOCK_SIZE, 1, jpg_file);
        }
        else
        {
            // 開いているファイルがあれば続きのデータを書き込む
            if (is_opened)
            {
                fwrite(buffer, BLOCK_SIZE, 1, jpg_file);
            }
        }
    }
    
    // 1回でもjpgファイルが開かれていたら、最後のファイルを閉じる
    if (file_count > 0 &&
        is_opened ==true)
    {
        fclose(jpg_file);
    }

    // ファイルを閉じて正常終了
    fclose(raw_file);
    return 0;
}
