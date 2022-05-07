#pragma once

#pragma pack(1)

#include <windows.h>
#include <iostream>
#include <fstream>

#include "StructBitMap.h"
#include "StructKaoSwap.h"

using namespace std;


extern int Cnv8BmpPixelTo8TSPixel(int []);


template <int BUSHOU_NUM>	// 1332=通常時の武将の数。1932人時など用に変更できるようテンプレートへ
class BitMapToKaoSwapManager {

  public:
	enum ErrorType { NoErr=0,  ErrNoTargetFile=-1, ErrWrongWHPixel=-2, ErrOutOfRange=-3, ErrColorPalette=-5, ErrNoKaoSwap=-6, ErrDefaultKaoswap=-8 };

  private:
	char szBMPFaceFileName[512];		// 0000.bmp 0000は番号 0001スタート
	char szBMPFaceFileDir[512];     // 

	
	struct BITMAPHEADER {			// ビットマップ情報とりだし用のヘッダ構造体。
		BITMAPFILEHEADER bfh;
		BITMAPINFOHEADER bih;
	};

	// 元のBMPを読み込む際の一時バッファ。
	// 実際には、おそらくは、2678バイトになるケースが最も多い。
	// 一応、規格やツールによって多少上下する
	// (又、時代とともに、同じ画像内容でもヘッダバージョンがあがってファイルサイズが増えることがあるはずなので、最大限安全牌。)
	// 又、いくら何でも、このサイズより大きいのは変なのでエラー
	char bufBMPFaceFile[3000];


	BITMAP_TENSHOUKI_FACE orgBmpFace;	// カレントターゲットの武将のBMPファイルバイナリ


  private:
	BYTE isFileExistOfTargetBushou[BUSHOU_NUM];		// ターゲットとなる武将のBMPは存在したか？

	KAOSWAP_TENSHOUKI<BUSHOU_NUM>	dstKaoSwap;		// KAOSWAP  を構築するためのバイナリ
	char szKaoswapFileName[512];						// KAOSWAPのファイルネーム


  public:
	BitMapToKaoSwapManager(char* szKaoSwapName) {

		// ファイルがあるのか無いのかのリスト初期化
		memset( isFileExistOfTargetBushou, 0, BUSHOU_NUM );

		// 顔は全て0xFF状態にする。全ての顔は真っ白
		memset( dstKaoSwap.face, 0xFF, sizeof(dstKaoSwap.face) );


		// kaoswap.n6pとかkaoswapex.n6pとか対象の名前を保存しておく
		strcpy_s(szKaoswapFileName, szKaoSwapName);

		char szTmpBuf[512] = "";
		strcpy_s(szTmpBuf, szKaoSwapName);
		PathRemoveExtension(szTmpBuf);
		strcpy_s(szBMPFaceFileDir, szTmpBuf);
	}

	// 全体の処理実行
	ErrorType Execute(int iFileNo) {

		ErrorType iErrorID;

		// ファイルを開いて、バッファへと格納
		iErrorID = OpenBmpFile( iFileNo );

		// エラーが無ければ…
		if ( !iErrorID ) {
			// バッファに読み込んだイメージをBMP構造体へと格納する。
			iErrorID = ToCopyBmpStruct();
			if ( iErrorID == ErrWrongWHPixel ) {
				cout << endl << szBMPFaceFileName << ":" << "ErrWrongWidthHeightPixel" << endl;
			}
		}

		// エラーが無ければ…
		// この中だけエラータイプ文字列……フッ…
		if ( !iErrorID ) {
			try {
				// 変換する
				CnvBmpToKaoswap( iFileNo );
			} catch (char *szErrMsg) {
				cout << endl << szBMPFaceFileName << ":" << szErrMsg << endl;
			}
		}

		return iErrorID;
	}

	// 顔スワップファイルを読んでメンバに格納する。
	ErrorType ReadKaoSwap() {
		cout << endl << endl << szKaoswapFileName << ":" << endl;

		fstream inKaoSwap;

		// これが無いと、なぜかファイルが認識されないことがある…
		DWORD dwAttr = ::GetFileAttributes(szKaoswapFileName);
		if(dwAttr == 0xffffffff) {
			cout << endl << "NoExist:" << szKaoswapFileName << endl;
			return ErrNoKaoSwap;
		}


		inKaoSwap.open(szKaoswapFileName, ios::in|ios::binary);

		if (!inKaoSwap) { 
			return ErrNoKaoSwap;
		}

		// 顔スワップ構造体へとコピー
		inKaoSwap.read( (char *)(&dstKaoSwap), sizeof(dstKaoSwap) );

		// ファイルサイズを調べる
		size_t fileSize = (size_t)inKaoSwap.seekg(0, ios::end).tellg();

		
		return NoErr;

	}

	// 顔スワップファイルをメンバからまんま書き出す。
	ErrorType WriteKaoSwap() {
		

		fstream outKaoSwap;

		// これが無いと、なぜかファイルが認識されないことがある…
		DWORD dwAttr = ::GetFileAttributes(szKaoswapFileName);

		if(dwAttr == 0xffffffff) {
			cout << endl<< szKaoswapFileName << ":NoExist" << endl;
			return ErrNoKaoSwap;
		}

		outKaoSwap.open(szKaoswapFileName, ios::out|ios::binary);

		if (!outKaoSwap) { return ErrNoKaoSwap; }

		// 顔スワップへと書き込み
		outKaoSwap.write( (char *)(&dstKaoSwap), sizeof(dstKaoSwap) );
		return NoErr;

	}


	// 指定の武将のBMPファイルを１つ開く～バッファへ格納まで
	ErrorType OpenBmpFile(int iFileNo) {

		if (iFileNo <= 0 || BUSHOU_NUM < iFileNo) { return ErrOutOfRange; }
		// 番号→ファイル名への変換
		wsprintf(szBMPFaceFileName, "%s\\%04d.bmp", szBMPFaceFileDir, iFileNo);

		// これが無いと、なぜかファイルが認識されないことがある…
		DWORD dwAttr = ::GetFileAttributes(szBMPFaceFileName);

		if(dwAttr != 0xffffffff) {
			cout << ".";
		}

		fstream fin;
		fin.open(szBMPFaceFileName, ios::in|ios::binary);

		// ファイルが存在しなければ…
		if (!fin) {
			// 該当の武将顔番号のファイルは無い。こっちは0オリジン配列なので-1すること。
			isFileExistOfTargetBushou[iFileNo-1] = 0;
			return ErrNoTargetFile; // このファイルは存在しない。
		}

		// バッファへと読み込み
		fin.read( bufBMPFaceFile, sizeof(bufBMPFaceFile) );

		// バッファにコピったのですぐ閉じる
		fin.close();

		return NoErr;
	}

	// ビットマップ構造体への格納まで
	ErrorType ToCopyBmpStruct() {

		// ビットマップヘッダーのインスタンス
		BITMAPHEADER *BitMapHeader;

		// バッファの先頭をBMPヘッダー情報ポインタとして扱う。
		BitMapHeader = (BITMAPHEADER *)bufBMPFaceFile;

		// 横64, 縦80かをチェック
		if ( BitMapHeader->bih.biWidth!=64 || BitMapHeader->bih.biHeight!=80 ) {
			return ErrWrongWHPixel; // このファイルは画素サイズが異なる		
		}

		// 画像内容が始まっている場所を特定し、そこから画像内容をBMP構造体用の格納場所にコピー
		int bfOffBits = BitMapHeader->bfh.bfOffBits;
		memcpy( &orgBmpFace, (bufBMPFaceFile+bfOffBits), sizeof(orgBmpFace) );

		return NoErr;

	}

	ErrorType CnvBmpToKaoswap(int iFileNo) {

		// 格納先を指しておく。変換する度に３バイトずつ進めなければならない。
		// 配列は0オリジンなので｢-1｣注意
		char *pface = (char *)&(dstKaoSwap.face[iFileNo-1]);

		// 顔画像の元のＢＭＰでは、画像内容は、一番最後のラインから最初のラインへと向けて並んでいる。
		// 一方、天翔記では、画像の一番最初から最後に向けて、並んでいる。
		// よって、ＢＭＰの最後から最初へ向けて計算し、天翔記用へと格納してゆく。
		for ( int iLineIX = FACE_LINE_NUM-1; iLineIX>=0; iLineIX--) {

			// 各パックごと変換。64ピクセル÷8で8つある。
			for (int iPackIX=0; iPackIX<(64/8); iPackIX++) {

				// ちょっと長いので参照で…
				BIT8PIXEL_PACK &bit8pack = orgBmpFace.bit64pixel_line[iLineIX].bit8pixel_pack[iPackIX];

				// 該当のBMPの８つのピクセルバイナリ情報を天翔記タイプの４バイト(うち上位１バイトは00)へと変換して返す
				int arrPixel[] = {
					bit8pack.pixel1,
					bit8pack.pixel2,
					bit8pack.pixel3,
					bit8pack.pixel4,
					bit8pack.pixel5,
					bit8pack.pixel6,
					bit8pack.pixel7,
					bit8pack.pixel8,
				};
				// 変換
				try {
					int iPackCnvResult = Cnv8BmpPixelTo8TSPixel(arrPixel);

					// まるまるその内容をコピーするのではなく、3バイト分だけコピー
					memcpy(pface, &iPackCnvResult, 3);

					// ３バイト分だけ進ませる
					pface += 3;

				} catch (char *szErrMsg) {
					throw szErrMsg;
				}
			}
		}

		return NoErr;
	}

};
