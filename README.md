# FindUsingProcess
引数で指定したファイル、フォルダ内のファイルを使用中としてロックしているプロセスを検索します。 
![サンプル画像](img/img1.png) 
  
## 特徴
* システム再起動、およびインストール工程で使用される Win32 APIを使用しています
* 特別なライブラリを使用していません 
  
## 使用例
* カレントフォルダ以下の全てのファイルの使用状況を調査します。  
  ```
  FindUsingProcess .
  ```
* カレントフォルダ以下の xlsx のファイルの使用状況を調査します（ワイルドカードを使用）  
  ```
  FindUsingProcess *.xlsx
  ```
* 指定のファイルの使用状況を調査します（フルパスを使用）  
  ```
  FindUsingProcess C:\～\ExcelTest\test.xlsx
  ```
  
## インストール
ソースをビルドして `FindUsingProcess.exe` ファイルを生成して参照してください。 
  
## 作成情報
* 造田　崇（zoppa software）
* ミウラ第1システムカンパニー 
* takashi.zouta@kkmiuta.jp
  
## ライセンス
[apache 2.0](https://www.apache.org/licenses/LICENSE-2.0.html)