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

// �ϊ���̊�f�[�^�̎��[�ꏊ
BMP_INDEX_COLOR8_FACE_DATA *bushou_face_data;

// �Ώۂ̃t�@�C���́A���l�̕����t�@�C���̃f�[�^�Ȃ̂��H
int GetBushouNum(String^ strInputFileName) {
	// FileInfo �̐V�����C���X�^���X�𐶐�����
	FileInfo ^cFileInfo = gcnew FileInfo(strInputFileName);

	// �t�@�C���T�C�Y���擾����
	int64_t lFileSize = cFileInfo->Length;

	// �t�@�C���T�C�Y���P�l������̃o�C�g���Ŋ������畐���̐l�����o��
	if (lFileSize % ONE_BUSHOU_BYTES == 0) {
		return static_cast<int>(lFileSize / ONE_BUSHOU_BYTES);
	}
	else {
		System::Windows::Forms::MessageBox::Show(strInputFileName + "�̃t�@�C���T�C�Y�́A�l���{�ɂȂ��Ă��܂���B\n���l�̃f�[�^�ł������Ɖ��肵�Čv�Z���Ă��s���ȃt�@�C���T�C�Y�ł��B");
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

	// �o�C�i��
	ifstream fin(szInputFileName, ios::in | ios::binary);

	// �t�@�C��������
	if (!fin){
		System::Windows::Forms::MessageBox::Show(strInputFileName + " ������");
		return false;
	}

	int iPackedColorIndexData;  // �R�o�C�g�Ƀp�b�N���ꂽ�f�[�^

	// �S���̊�f�[�^���A�S�o�C�g�̃s�N�Z���p�b�N�ŁA���������Ă����B
	BIT8PIXEL_PACK *bp = (BIT8PIXEL_PACK *)bushou_face_data;

	int count = 0;
	while (!fin.eof()){ 
		// 3�o�C�g���p�b�N����Ă�̂ŁA�R�o�C�g���ǂݍ���
		fin.read((char *)&iPackedColorIndexData, 3);
		// �R�o�C�g�̃p�b�N���ꂽ�f�[�^�˂W�̃C���f�b�N�X���l��
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

	// ���̂܂܂���BMP���㉺���]���Ă��܂��Ă���̂ŁA�㉺���]�𒼂��B
	for (int b = 0; b < KaoDataPFaceViewerForm::BUSHOU_NUM; b++) {

		BMP_INDEX_COLOR8_FACE_DATA face_tmp_data;
		BMP_INDEX_COLOR8_FACE_DATA *pFace = (BMP_INDEX_COLOR8_FACE_DATA *)&bushou_face_data[b];

		// ��l������Ń��C���𔽓]
		for (int l = 0; l < 80; l++) {
			face_tmp_data.line[l] = pFace->line[79 - l];
		}

		// ���]�������̂����ւƏ㏑��
		memcpy(pFace->data, &face_tmp_data.data, sizeof(face_tmp_data));
	}

	return true;
}

// �o�C�g�z����C���[�W�摜�ւƕϊ�
Image ^ConvertByteArrayToImage(cli::array<byte> ^byte_array)
{
	ImageConverter ^imgconv = gcnew ImageConverter();
	Image ^img = safe_cast<Image^>(imgconv->ConvertFrom(byte_array));
	return img;
}

// �C���[�W�摜���o�C�g�z���
cli::array<byte>^ ConvertImageToByteArray(Image ^img)
{
	ImageConverter ^imgconv = gcnew ImageConverter();
	cli::array<byte>^ b = safe_cast<cli::array<byte>^>(imgconv->ConvertTo(img, cli::array<byte>::typeid));
	return b;
}

// �w��̕����ԍ�����A�摜���擾
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
	// �R���g���[�����쐬�����O�ɁAWindows XP �r�W���A�����ʂ�L���ɂ��܂�
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	String^ strTargetFile = "kaodatap.s5";
	if (args->Length > 0) {
		strTargetFile = args[0];
	}

	if (!File::Exists(strTargetFile)) {
		System::Windows::Forms::MessageBox::Show(strTargetFile + " ������");
		return -1;
	}

	int iBushouNum = GetBushouNum(strTargetFile);

	// �����̐l�����s��
	if (iBushouNum < 0) {
		return -1;
	}

	KaoDataPFaceViewerForm::BUSHOU_NUM = iBushouNum;

	MallocBushouFaceDataBuffer();

	// ���C�� �E�B���h�E���쐬���āA���s���܂�
	Application::Run(gcnew KaoDataPFaceViewerForm(strTargetFile));

	FreeBushouFaceDataBuffer();
}