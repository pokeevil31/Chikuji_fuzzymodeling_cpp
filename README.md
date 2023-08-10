# Chikuji_fuzzymodeling_cpp
C++による逐次法ファジィモデリング．

## すぐに使えるための説明

`Microsoft Visual Studio Community 2022 (64-bit) Version 17.0.2`にてプロジェクトで実装した．

Visual Studioを使えばすぐ実行できると思う．

2023.07.03現在のバージョンをそのまま実行すれば放物面をモデリングし，何番目のルールが追加されているかと縮小の情報などが表示される．
モデリングの結果はCSVファイルに保存され，この部分については以下で説明する．

### プロットとcsvについて
csvファイルにはモデリング結果が保存されている．ファイル名は`main.cpp`内の`file_result`で指定している．

プロットするには**別のプログラム**が必要．

プロット用のPythonプログラムはこちらに用意してある：
https://github.com/pokeevil31/plot_csv_fuzzymodel_py.git

### モデリング用のデータ
2023.07.03現在，`int build_data(double** &x, double* &y, int dim)`と`void data_func(double**& x, double*& y, int size, int dim)`でそれぞれ**入力**と**出力**を指定してデータを作っている．

#### `int build_data`について
`x_min`から`x_max`から`each_size`等分するように入力データを生成する．`dim`は生成する入力データの次元数を指定する．

例えば`x_min=0`,`x_max=1`,`each_size=1`,`dim=2`で生成される入力データは以下のようになる．

>size = 4
>
>x[0][0] = 0.0, x[0][1] = 0.0
>
>x[1][0] = 1.0, x[1][1] = 0.0
>
>x[2][0] = 0.0, x[2][1] = 1.0
>
>x[3][0] = 1.0, x[3][1] = 1.0

`void data_func`が設定されている場合，関数の最後に`// check the data`のところでコメントアウトされている部分のコメントアウトを解除すれば，生成された入力と出力を確認することができる．

#### `void data_func`について

`int build_data`内で使用される出力を計算する関数．変数の`double**& x`と`double*& y`はあらかじめメモリ確保する必要がある．

計算する関数を設定したい場合は`if (dim == 1)`と`else if (dim == 2)`の部分でそれぞれ一変数関数と二変数関数を指定してください．三変数以上も対応可能ですが，プロットによる確認が困難なため，現在実装していない．
