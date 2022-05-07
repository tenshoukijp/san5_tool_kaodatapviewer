#pragma once

#pragma pack(1)

#include <windows.h>
#include <iostream>
#include <fstream>

#include "StructBitMap.h"
#include "StructKaoSwap.h"

using namespace std;


extern int Cnv8BmpPixelTo8TSPixel(int []);


template <int BUSHOU_NUM>	// 1332=�ʏ펞�̕����̐��B1932�l���ȂǗp�ɕύX�ł���悤�e���v���[�g��
class BitMapToKaoSwapManager {

  public:
	enum ErrorType { NoErr=0,  ErrNoTargetFile=-1, ErrWrongWHPixel=-2, ErrOutOfRange=-3, ErrColorPalette=-5, ErrNoKaoSwap=-6, ErrDefaultKaoswap=-8 };

  private:
	char szBMPFaceFileName[512];		// 0000.bmp 0000�͔ԍ� 0001�X�^�[�g
	char szBMPFaceFileDir[512];     // 

	
	struct BITMAPHEADER {			// �r�b�g�}�b�v���Ƃ肾���p�̃w�b�_�\���́B
		BITMAPFILEHEADER bfh;
		BITMAPINFOHEADER bih;
	};

	// ����BMP��ǂݍ��ލۂ̈ꎞ�o�b�t�@�B
	// ���ۂɂ́A�����炭�́A2678�o�C�g�ɂȂ�P�[�X���ł������B
	// �ꉞ�A�K�i��c�[���ɂ���đ����㉺����
	// (���A����ƂƂ��ɁA�����摜���e�ł��w�b�_�o�[�W�������������ăt�@�C���T�C�Y�������邱�Ƃ�����͂��Ȃ̂ŁA�ő�����S�v�B)
	// ���A�����牽�ł��A���̃T�C�Y���傫���͕̂ςȂ̂ŃG���[
	char bufBMPFaceFile[3000];


	BITMAP_TENSHOUKI_FACE orgBmpFace;	// �J�����g�^�[�Q�b�g�̕�����BMP�t�@�C���o�C�i��


  private:
	BYTE isFileExistOfTargetBushou[BUSHOU_NUM];		// �^�[�Q�b�g�ƂȂ镐����BMP�͑��݂������H

	KAOSWAP_TENSHOUKI<BUSHOU_NUM>	dstKaoSwap;		// KAOSWAP  ���\�z���邽�߂̃o�C�i��
	char szKaoswapFileName[512];						// KAOSWAP�̃t�@�C���l�[��


  public:
	BitMapToKaoSwapManager(char* szKaoSwapName) {

		// �t�@�C��������̂������̂��̃��X�g������
		memset( isFileExistOfTargetBushou, 0, BUSHOU_NUM );

		// ��͑S��0xFF��Ԃɂ���B�S�Ă̊�͐^����
		memset( dstKaoSwap.face, 0xFF, sizeof(dstKaoSwap.face) );


		// kaoswap.n6p�Ƃ�kaoswapex.n6p�Ƃ��Ώۂ̖��O��ۑ����Ă���
		strcpy_s(szKaoswapFileName, szKaoSwapName);

		char szTmpBuf[512] = "";
		strcpy_s(szTmpBuf, szKaoSwapName);
		PathRemoveExtension(szTmpBuf);
		strcpy_s(szBMPFaceFileDir, szTmpBuf);
	}

	// �S�̂̏������s
	ErrorType Execute(int iFileNo) {

		ErrorType iErrorID;

		// �t�@�C�����J���āA�o�b�t�@�ւƊi�[
		iErrorID = OpenBmpFile( iFileNo );

		// �G���[��������΁c
		if ( !iErrorID ) {
			// �o�b�t�@�ɓǂݍ��񂾃C���[�W��BMP�\���̂ւƊi�[����B
			iErrorID = ToCopyBmpStruct();
			if ( iErrorID == ErrWrongWHPixel ) {
				cout << endl << szBMPFaceFileName << ":" << "ErrWrongWidthHeightPixel" << endl;
			}
		}

		// �G���[��������΁c
		// ���̒������G���[�^�C�v������c�c�t�b�c
		if ( !iErrorID ) {
			try {
				// �ϊ�����
				CnvBmpToKaoswap( iFileNo );
			} catch (char *szErrMsg) {
				cout << endl << szBMPFaceFileName << ":" << szErrMsg << endl;
			}
		}

		return iErrorID;
	}

	// ��X���b�v�t�@�C����ǂ�Ń����o�Ɋi�[����B
	ErrorType ReadKaoSwap() {
		cout << endl << endl << szKaoswapFileName << ":" << endl;

		fstream inKaoSwap;

		// ���ꂪ�����ƁA�Ȃ����t�@�C�����F������Ȃ����Ƃ�����c
		DWORD dwAttr = ::GetFileAttributes(szKaoswapFileName);
		if(dwAttr == 0xffffffff) {
			cout << endl << "NoExist:" << szKaoswapFileName << endl;
			return ErrNoKaoSwap;
		}


		inKaoSwap.open(szKaoswapFileName, ios::in|ios::binary);

		if (!inKaoSwap) { 
			return ErrNoKaoSwap;
		}

		// ��X���b�v�\���̂ւƃR�s�[
		inKaoSwap.read( (char *)(&dstKaoSwap), sizeof(dstKaoSwap) );

		// �t�@�C���T�C�Y�𒲂ׂ�
		size_t fileSize = (size_t)inKaoSwap.seekg(0, ios::end).tellg();

		
		return NoErr;

	}

	// ��X���b�v�t�@�C���������o����܂�܏����o���B
	ErrorType WriteKaoSwap() {
		

		fstream outKaoSwap;

		// ���ꂪ�����ƁA�Ȃ����t�@�C�����F������Ȃ����Ƃ�����c
		DWORD dwAttr = ::GetFileAttributes(szKaoswapFileName);

		if(dwAttr == 0xffffffff) {
			cout << endl<< szKaoswapFileName << ":NoExist" << endl;
			return ErrNoKaoSwap;
		}

		outKaoSwap.open(szKaoswapFileName, ios::out|ios::binary);

		if (!outKaoSwap) { return ErrNoKaoSwap; }

		// ��X���b�v�ւƏ�������
		outKaoSwap.write( (char *)(&dstKaoSwap), sizeof(dstKaoSwap) );
		return NoErr;

	}


	// �w��̕�����BMP�t�@�C�����P�J���`�o�b�t�@�֊i�[�܂�
	ErrorType OpenBmpFile(int iFileNo) {

		if (iFileNo <= 0 || BUSHOU_NUM < iFileNo) { return ErrOutOfRange; }
		// �ԍ����t�@�C�����ւ̕ϊ�
		wsprintf(szBMPFaceFileName, "%s\\%04d.bmp", szBMPFaceFileDir, iFileNo);

		// ���ꂪ�����ƁA�Ȃ����t�@�C�����F������Ȃ����Ƃ�����c
		DWORD dwAttr = ::GetFileAttributes(szBMPFaceFileName);

		if(dwAttr != 0xffffffff) {
			cout << ".";
		}

		fstream fin;
		fin.open(szBMPFaceFileName, ios::in|ios::binary);

		// �t�@�C�������݂��Ȃ���΁c
		if (!fin) {
			// �Y���̕�����ԍ��̃t�@�C���͖����B��������0�I���W���z��Ȃ̂�-1���邱�ƁB
			isFileExistOfTargetBushou[iFileNo-1] = 0;
			return ErrNoTargetFile; // ���̃t�@�C���͑��݂��Ȃ��B
		}

		// �o�b�t�@�ւƓǂݍ���
		fin.read( bufBMPFaceFile, sizeof(bufBMPFaceFile) );

		// �o�b�t�@�ɃR�s�����̂ł�������
		fin.close();

		return NoErr;
	}

	// �r�b�g�}�b�v�\���̂ւ̊i�[�܂�
	ErrorType ToCopyBmpStruct() {

		// �r�b�g�}�b�v�w�b�_�[�̃C���X�^���X
		BITMAPHEADER *BitMapHeader;

		// �o�b�t�@�̐擪��BMP�w�b�_�[���|�C���^�Ƃ��Ĉ����B
		BitMapHeader = (BITMAPHEADER *)bufBMPFaceFile;

		// ��64, �c80�����`�F�b�N
		if ( BitMapHeader->bih.biWidth!=64 || BitMapHeader->bih.biHeight!=80 ) {
			return ErrWrongWHPixel; // ���̃t�@�C���͉�f�T�C�Y���قȂ�		
		}

		// �摜���e���n�܂��Ă���ꏊ����肵�A��������摜���e��BMP�\���̗p�̊i�[�ꏊ�ɃR�s�[
		int bfOffBits = BitMapHeader->bfh.bfOffBits;
		memcpy( &orgBmpFace, (bufBMPFaceFile+bfOffBits), sizeof(orgBmpFace) );

		return NoErr;

	}

	ErrorType CnvBmpToKaoswap(int iFileNo) {

		// �i�[����w���Ă����B�ϊ�����x�ɂR�o�C�g���i�߂Ȃ���΂Ȃ�Ȃ��B
		// �z���0�I���W���Ȃ̂Ţ-1�����
		char *pface = (char *)&(dstKaoSwap.face[iFileNo-1]);

		// ��摜�̌��̂a�l�o�ł́A�摜���e�́A��ԍŌ�̃��C������ŏ��̃��C���ւƌ����ĕ���ł���B
		// ����A�V�ċL�ł́A�摜�̈�ԍŏ�����Ō�Ɍ����āA����ł���B
		// ����āA�a�l�o�̍Ōォ��ŏ��֌����Čv�Z���A�V�ċL�p�ւƊi�[���Ă䂭�B
		for ( int iLineIX = FACE_LINE_NUM-1; iLineIX>=0; iLineIX--) {

			// �e�p�b�N���ƕϊ��B64�s�N�Z����8��8����B
			for (int iPackIX=0; iPackIX<(64/8); iPackIX++) {

				// ������ƒ����̂ŎQ�ƂŁc
				BIT8PIXEL_PACK &bit8pack = orgBmpFace.bit64pixel_line[iLineIX].bit8pixel_pack[iPackIX];

				// �Y����BMP�̂W�̃s�N�Z���o�C�i������V�ċL�^�C�v�̂S�o�C�g(������ʂP�o�C�g��00)�ւƕϊ����ĕԂ�
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
				// �ϊ�
				try {
					int iPackCnvResult = Cnv8BmpPixelTo8TSPixel(arrPixel);

					// �܂�܂邻�̓��e���R�s�[����̂ł͂Ȃ��A3�o�C�g�������R�s�[
					memcpy(pface, &iPackCnvResult, 3);

					// �R�o�C�g�������i�܂���
					pface += 3;

				} catch (char *szErrMsg) {
					throw szErrMsg;
				}
			}
		}

		return NoErr;
	}

};
