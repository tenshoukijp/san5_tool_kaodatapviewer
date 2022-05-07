#include <iostream>

#include <vector>
#include <map>

#include "BitMapToKaoSwapCoreEngine.h"

using namespace std;

//----------------以下は天翔記用KaoSwap内の変換規則のコア中のコア。
//                ここさえ抑えていれば、いくらでも別のツールが作りやすい。

/*
天翔記では、顔画像は横８ピクセルで１つの値として扱う。
横８ピクセルのうち、一番右のピクセルに１つその色で塗られていると以下の値となる
これを基礎値とでも命名する。
古い時代のゲームで、データサイズを圧縮するために、下記のような組み合わせによって(ビットシフトに耐える)８つの基礎カラーを実現したのであろう。
下のように１つ塗られているピクセルが左へ１つ移動するたびに、１つビットシフトされる。
[ □□□□□□□■ ] の■１つだけが、対象の色で塗られていた場合の値＝基礎値。
[ □□□□□■□□ ] 左のように２つ移動すると、基礎値 << 2 というように２つビットシフト演算することとなる。
*/
#define iKaoswapColorIndexBaseCnt	8
int KaoswapColorIndexBase[iKaoswapColorIndexBaseCnt] = {
	0x000000, // 黒  R=0   G=0   B=0 
	0x000001, // 青	 R=17  G=68  B=153 
	0x000100, // 赤	 R=187 G=68  B=0 
	0x000101, // 橙	 R=204 G=136 B=68 
	0x010000, // 緑	 R=0   G=119 B=51 
	0x010001, // 空	 R=85  G=153 B=170 
	0x010100, // 黄	 R=255 G=187 B=85 
	0x010101, // 白  R=238 G=238 B=204 
};

/*
８つ横に並んだインデックスピクセル情報を、天翔記のint値に直す。
INDEXタイプのBMPは本当にカラーINDEXがバイトとして１つずつ入ってるだけなので楽勝である。
bmpでは以下のようにインデックスからーが入っている、
[ ０２３０３６０１ ] みたいに８ピクセルに対応するカラーインデックスが１つずつ入っているだけ。
(※256色パレットでは、実際にこの通りのインデックスカラー情報がバイトごとに入っている)
(※16 色パレットでは、これが圧縮されて、１バイトに２つずつ[ 02 30 36 01 ]みたいに入っているので以下の関数にはこれを分解して渡すこと)

bmpを分解し、これを天翔記タイプへと変換する。
カラーのインデックスから天翔記においての基礎値(上のColorIndexBase)を得て、何番目のピクセルかに応じて
Shift演算すれば良いだけである。

天翔記では3バイト情報で１塊となるので、リターン値のint(4バイト全部が利用されるわけではない)
最も大きい部分(リトルエンディアンでメモリ配置上では一番右)は常に00。要すに、
返り値int = [xx xx xx 00]  xx xx xx の部分が天翔記のデータとして使われる。
*/

#define iPixelPack	8

// iColorIndex[8] → ８つの横に並んだピクセルの情報、中身は各々のインデックスカラー
int Cnv8BmpPixelTo8TSPixel(int iColorIndex[iPixelPack]) {
	// ８つのピクセルのカラーインデックス値が入っている
	int iReturn8TSPixcelValue = 0; // 0はデフォルトの黒。８つの横にならんだピクセルが黒状態

	for (int ix = 0; ix<iPixelPack; ix++) {

		// カラーインデックス値のオーバーフローチェック
		int cIX = iColorIndex[ix];

		if (cIX >= iKaoswapColorIndexBaseCnt) {
			throw "ErrUsedColorPaletteOutOfRange!!"; // 

		}
		else {
			// 該当のカラーインデックス値の基礎値*(7-Shift値)
			// 8つ並んだピクセル情報のうち、一番左なら7つ基礎値よりビットシフトされたものとなる。
			iReturn8TSPixcelValue += KaoswapColorIndexBase[cIX] << (7 - ix);
		}

	}
	return iReturn8TSPixcelValue;
}




map<int, int> reverseHash;

// 天翔記の顔などに使われている８ピクセルのパックされた情報⇒8つのindex数値へ
// ここ実行の回数が非常に多いので、vectorなどは利用せず、実体が１つのint64で済むRESULT_8BMP_PIXEL型にしている。
RESULT_8BMP_PIXEL Cnv8TSPixelTo8BmpPixel(int iTsColorPackedIndex) {

	// カラーマップが出来ていなければ、逆ハッシュを構成
	if (reverseHash.size() == 0) {
		for (int i = 0; i < iKaoswapColorIndexBaseCnt; i++) {
			int value = KaoswapColorIndexBase[i];
			reverseHash[value] = i;
		}

	}

	// ８つのピクセルのカラーインデックス値が入っている
	RESULT_8BMP_PIXEL BmpColorIndexList;

	int bitFilter = KaoswapColorIndexBase[7]; // 最後の値が 0b [00000001][00000001][00000001] というフィルター

	for (int ix = 0; ix < iPixelPack; ix++) {

		int bitShift = (7 - ix);
		int filter = bitFilter << bitShift;
		int iPackedIndex = iTsColorPackedIndex & filter;
		int iHashKey = iPackedIndex >> bitShift;

		BmpColorIndexList.at[ix] = reverseHash[iHashKey];
	}
	return BmpColorIndexList;
}
