#include "helpers.h"
#include <math.h> // round利用のため

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // 各ピクセルのrgbの値を初期化
    int red_pixel = 0;
    int green_pixel = 0;
    int blue_pixel = 0;

    // 1ピクセルをグレースケールするにあたって必要な値を定義
    int sum_pixel = 0;
    int grayscale_pixel = 0;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // 元イメージのコピーを取得
            RGBTRIPLE one_pixel = image[i][j];

            // 1ピクセルのグレースケールの値を算出
            sum_pixel = one_pixel.rgbtRed + one_pixel.rgbtGreen + one_pixel.rgbtBlue;
            grayscale_pixel = round(sum_pixel / 3.0); // round内の計算結果を小数も含める必要があるため

            // グレースケールの値をr,g,bの値に入れる
            one_pixel.rgbtRed = grayscale_pixel;
            one_pixel.rgbtGreen = grayscale_pixel;
            one_pixel.rgbtBlue = grayscale_pixel;

            // 変更したコピーの値を元イメージに反映させる
            image[i][j] = one_pixel;
        }
    }
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    const int RGB_MAX = 255;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // 元イメージのコピーを取得
            RGBTRIPLE one_pixel = image[i][j];

            // 各カラーのセピアカラーの値を算出
            int sepiaRed = .393 * one_pixel.rgbtRed + .769 * one_pixel.rgbtGreen + .189 * one_pixel.rgbtBlue;
            int sepiaGreen = .349 * one_pixel.rgbtRed + .686 * one_pixel.rgbtGreen + .168 * one_pixel.rgbtBlue;
            int sepiaBlue = .272 * one_pixel.rgbtRed + .534 * one_pixel.rgbtGreen + .131 * one_pixel.rgbtBlue;

            // r,g,bの値は255が上限のため、上限を超える場合は、各セピアカラーの値の上限値を設定
            if (sepiaRed > RGB_MAX)
            {
                sepiaRed = RGB_MAX;
            }
            if (sepiaGreen > RGB_MAX)
            {
                sepiaGreen = RGB_MAX;
            }
            if (sepiaBlue > RGB_MAX)
            {
                sepiaBlue = RGB_MAX;
            }

            // 各セピアカラーの値をr,g,bの値に入れる
            one_pixel.rgbtRed = sepiaRed;
            one_pixel.rgbtGreen = sepiaGreen;
            one_pixel.rgbtBlue = sepiaBlue;

            // 変更したコピーの値を元イメージに反映させる
            image[i][j] = one_pixel;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        // 行ごとに行の外側からピクセルのスワップを行う
        for (int j = 0; j < width / 2; j++)
        {
            // 0とwidth - 1, 0とwidth -2, 3とwidth -3...のように入れ替えていく
            RGBTRIPLE temp_pixel = image[i][j];
            image[i][j] = image[i][width - 1 -j];
            image[i][width - 1 -j] = temp_pixel;
        }
    }
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // 集計するr,g,bの合計値を格納するための変数群の定義
            int sum_red = 0;
            int sum_green = 0;
            int sum_blue = 0;

            // 対象ピクセルが何個あるかをカウントする用
            int pixel_count = 0;

            // 自身を含んだ3*3の範囲のピクセル情報を取得
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj < 3; dj++)
                {
                    // チェック対象のピクセルの座標・ぼかすピクセルの左上からスタート
                    int ni = i + di;
                    int nj = j + dj;

                    // 存在するピクセルを参照する場合
                    if ((ni >= 0 && ni < height) &&
                        (nj >= 0 && nj < width))
                    {
                        // 参照ピクセルのr,g,bの値を足していく
                        sum_red += image[ni][nj].rgbtRed;
                        sum_green += image[ni][nj].rgbtGreen;
                        sum_blue += image[ni][nj].rgbtBlue;

                        // 参照ピクセルをカウント
                        pixel_count++;
                    }
                }
            }

            // 参照ピクセルのr,g,bの値の平均値が、ぼかし対象のピクセルの新しいr,g,bの値となる
            image[i][j].rgbtRed = round((double) sum_red / pixel_count);
            image[i][j].rgbtGreen = round((double) sum_green / pixel_count);
            image[i][j].rgbtBlue = round((double) sum_blue / pixel_count);
        }
    }
}
