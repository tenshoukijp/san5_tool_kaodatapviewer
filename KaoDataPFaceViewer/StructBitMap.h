#pragma once

#include <windows.h>

#pragma pack(1)
/*
	構造体とかで勝手に指定のビットとはことなるようにパッキングして整列しなおすことの防止。
	#pragma pack(1)とすることで、1bitずつの整列となるため、事実上、整列されなくなる。
*/


/*
以下はビットマップヘッダー

	オフセット	バイト数	構造体フィールド名	説明
	0			2			BfType				ファイルのタイプ、ビットマップを示すBM（0x424d）が入る。
	2			4			BfSize				ファイルのバイト数。ファイルのサイズの情報が入る。
	6			2			BfReserved1			予約域。値は、0x00が入る。
	8			2			BfReserved2			予約域。値は、0x00が入る。

	～～ここまでははっきりいってどうでもいい～～


	↓だけ重要というか、画像内容の開始点～～

	000A		4			BfOffBits			ファイルの先頭から、画像情報までのオフセット。先頭からなので、事実上、このアドレスから画像内容が始まっている。
	決めうちにしないのは、8パレット中8パレット使用のbmpファイルと16パレット中8パレット使用のbmpファイルとでは微妙にヘッダサイズが異なるため。
	神パッチの顔ツールはここの判断が抜けていて、取り回ししにくい。

struct BITMAPFILEHEADER;
{
	WORD    bfType; 
	DWORD   bfSize; 
	WORD    bfReserved1; 
	WORD    bfReserved2; 
	DWORD   bfOffBits; 
}
*/



/*
	オフセット	バイト数	構造体フィールド名	説明
	000E		4			BiSize				BITMAPINFO HEADER構造体のサイズ。
	12			4			BiWidth				画像の幅。単位はピクセル。
	16			4			BiHeight			画像の高さ。単位はピクセル。

struct BITMAPINFOHEADER;
{
	DWORD  biSize; 
	LONG   biWidth; 
	LONG   biHeight; 
	～～ここから下の要素はバージョンによって異なる
};
*/

//----------------以上が汎用のBitMapフォーマット




//----------------以下は汎用BitMap８色の天翔記顔(というか、パレットが８か１６中、先頭８色を利用した64*80のサイズのBMP)

/*
	以下３つで顔を取り扱いやすくするためのセット
	このようにするのは、bmpファイルは画像の左下の情報から、１ラインずつ上へと向かって情報が格納されているのに対して、
	天翔記では、左上から情報が格納されているため。
	ポインタ計算しても良いが、後々見やすくするため、又、そもそも両者の変換計算を自明とするため、このようにする。
	(天翔記の顔画像のフォーマットが64x80固定で決して変わらないため、ランタイム計算は無駄が多いということもある)
*/

/*
	計算に使う最小単位は1ピクセルの情報が4bitに入っていて、8ピクセルで4バイト
	これが1パック
	下記構造体で、１つ１つのビットフィールドに名前付きなのは、
	記述的には多少芋になるが、リトルエンディアンによるひっくり返し等、いろいろあるので、
	計算量的には、ダイナミックに計算するよりも良い
*/
struct BIT8PIXEL_PACK
{
	BYTE pixel2:4;
	BYTE pixel1:4;
	BYTE pixel4:4;
	BYTE pixel3:4;
	BYTE pixel6:4;
	BYTE pixel5:4;
	BYTE pixel8:4;
	BYTE pixel7:4;
};

// 上が８パックで顔画像の横1列となる。
struct BIT64PIXEL_LINE
{
	BIT8PIXEL_PACK	bit8pixel_pack[8]; // 8Pixel が8つで64Pixel １ラインは64バイト
};

#define FACE_LINE_NUM		80

// 横1列が縦に80個で顔画像が出来上がる。
struct BITMAP_TENSHOUKI_FACE
{
	BIT64PIXEL_LINE bit64pixel_line[FACE_LINE_NUM]; // 縦は80line存在する。
};


