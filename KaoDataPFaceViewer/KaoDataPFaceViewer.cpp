#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <vector>
#include <msclr/marshal_cppstd.h>

#include "KaoDataStruct.h"
#include "BitMapToKaoSwapCoreEngine.h"

#include "KaoDataPFaceViewerForm.h"

using namespace std;
using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace msclr::interop;

// 変換後の顔データの収納場所
BMP_INDEX_COLOR8_FACE_DATA *bushou_face_data;

// 対象のファイルは、何人の武将ファイルのデータなのか？
int GetBushouNum(String^ strInputFileName) {
	// FileInfo の新しいインスタンスを生成する
	FileInfo ^cFileInfo = gcnew FileInfo(strInputFileName);

	// ファイルサイズを取得する
	int64_t lFileSize = cFileInfo->Length;

	// ファイルサイズを１人当たりのバイト数で割ったら武将の人数が出る
	if (lFileSize % ONE_BUSHOU_BYTES == 0) {
		return static_cast<int>(lFileSize / ONE_BUSHOU_BYTES);
	}
	else {
		System::Windows::Forms::MessageBox::Show(strInputFileName + "のファイルサイズは、人数倍になっていません。\n何人のデータであったと仮定して計算しても不正なファイルサイズです。");
		return -1;
	}
}

void MallocBushouFaceDataBuffer() {
	bushou_face_data = new BMP_INDEX_COLOR8_FACE_DATA[KaoDataPFaceViewerForm::BUSHOU_NUM];
}

void FreeBushouFaceDataBuffer() {
	delete[] bushou_face_data;
}

bool ImportKaoDataFileToProgramStruct(String^ strInputFileName) {

	wstring szInputFileName = marshal_as<std::wstring>(strInputFileName);

	// バイナリ
	ifstream fin(szInputFileName, ios::in | ios::binary);

	// ファイルが無い
	if (!fin){
		System::Windows::Forms::MessageBox::Show(strInputFileName + " が無い");
		return false;
	}

	int iPackedColorIndexData;  // ３バイトにパックされたデータ

	// 全員の顔データを、４バイトのピクセルパックで、さし示しておく。
	BIT8PIXEL_PACK *bp = (BIT8PIXEL_PACK *)bushou_face_data;

	int count = 0;
	while (!fin.eof()){ 
		// 3バイトずつパックされてるので、３バイトずつ読み込み
		fin.read((char *)&iPackedColorIndexData, 3);
		// ３バイトのパックされたデータ⇒８つのインデックス数値へ
		RESULT_8BMP_PIXEL result = Cnv8TSPixelTo8BmpPixel(iPackedColorIndexData);

		bp->pixel1 = result.at[0];
		bp->pixel2 = result.at[1];
		bp->pixel3 = result.at[2];
		bp->pixel4 = result.at[3];
		bp->pixel5 = result.at[4];
		bp->pixel6 = result.at[5];
		bp->pixel7 = result.at[6];
		bp->pixel8 = result.at[7];

		bp++;
	}

	fin.close();

	// そのままだとBMPが上下反転してしまっているので、上下反転を直す。
	for (int b = 0; b < KaoDataPFaceViewerForm::BUSHOU_NUM; b++) {

		BMP_INDEX_COLOR8_FACE_DATA face_tmp_data;
		BMP_INDEX_COLOR8_FACE_DATA *pFace = (BMP_INDEX_COLOR8_FACE_DATA *)&bushou_face_data[b];

		// 一人当たりでラインを反転
		for (int l = 0; l < 80; l++) {
			face_tmp_data.line[l] = pFace->line[79 - l];
		}

		// 反転したものを元へと上書き
		memcpy(pFace->data, &face_tmp_data.data, sizeof(face_tmp_data));
	}

	return true;
}

// バイト配列をイメージ画像へと変換
Image ^ConvertByteArrayToImage(cli::array<byte> ^byte_array)
{
	ImageConverter ^imgconv = gcnew ImageConverter();
	Image ^img = safe_cast<Image^>(imgconv->ConvertFrom(byte_array));
	return img;
}

// イメージ画像をバイト配列へ
cli::array<byte>^ ConvertImageToByteArray(Image ^img)
{
	ImageConverter ^imgconv = gcnew ImageConverter();
	cli::array<byte>^ b = safe_cast<cli::array<byte>^>(imgconv->ConvertTo(img, cli::array<byte>::typeid));
	return b;
}

// 指定の武将番号から、画像を取得
Image^ GetBushouFaceImage(int iBushouID) {
	if (0 <= iBushouID && iBushouID < KaoDataPFaceViewerForm::BUSHOU_NUM) {
		BMP_INDEX_COLOR8_FACE_FILE_FORMAT face;
		face.header = binBitMapHeaderSan5;
		face.data = bushou_face_data[iBushouID];
		byte *pFace = (byte *)&face;

		auto managed_face_bmp_format_data = gcnew cli::array<byte>(sizeof(face));

		pin_ptr<byte> mng_pin_ptr = &managed_face_bmp_format_data[0];
		memcpy(mng_pin_ptr, &face, sizeof(face));

		Image^ img = ConvertByteArrayToImage(managed_face_bmp_format_data);
		return img;
	}
	else {
		return nullptr;
	}
}



[STAThreadAttribute]
int main(cli::array<System::String ^> ^args)
{
	// コントロールが作成される前に、Windows XP ビジュアル効果を有効にします
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	String^ strTargetFile = "kaodatap.s5";
	if (args->Length > 0) {
		strTargetFile = args[0];
	}

	if (!File::Exists(strTargetFile)) {
		System::Windows::Forms::MessageBox::Show(strTargetFile + " が無い");
		return -1;
	}

	int iBushouNum = GetBushouNum(strTargetFile);

	// 武将の人数が不正
	if (iBushouNum < 0) {
		return -1;
	}

	KaoDataPFaceViewerForm::BUSHOU_NUM = iBushouNum;

	MallocBushouFaceDataBuffer();

	// メイン ウィンドウを作成して、実行します
	Application::Run(gcnew KaoDataPFaceViewerForm(strTargetFile));

	FreeBushouFaceDataBuffer();
}