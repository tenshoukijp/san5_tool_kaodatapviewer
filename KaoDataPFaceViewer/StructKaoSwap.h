#pragma once

#pragma pack(1)

#include <windows.h>

//----------------以下は天翔記用KaoSwap内の画素情報

// 横1列が縦に80個で顔画像が出来上がる。
struct KAOSWAP_TENSHOUKI_FACE_64x80
{
	BYTE data[1920]; // 1920バイトが顔１つ。１ライン(64ピクセル)＝24バイトであるが、その管理は特に不要。
};



template <int BUSHOU_NUM>	// 1332=通常時の武将の数。1932や2932人時など用に変更できるようテンプレートへ
struct KAOSWAP_TENSHOUKI
{
	KAOSWAP_TENSHOUKI_FACE_64x80 face[BUSHOU_NUM];	// 顔画像内容。人数分
};

