# combo-bin

combo ナップサックソルバー[^combo]の薄いラッパー. 
入力ファイルからナップサック問題のインスタンスを読み取って combo ソルバーを実行する. 

[^combo]: Martello, Silvano and Pisinger, David and Toth, Paolo. 1999. Dynamic Programming and Strong Bounds for the 0-1 Knapsack Problem. Management Science. 45, 3, 414-424. https://doi.org/10.1287/mnsc.45.3.414

## Build

```bash
nix build
```

## Usage

```bash
combo [OPTIONS] [FILE]
```

入力ファイルは [combo の作者のページ](https://hjemmesider.diku.dk/~pisinger/codes.html)で配布されている問題インスタンスを想定している. 
