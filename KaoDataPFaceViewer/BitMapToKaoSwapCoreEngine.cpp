#include <iostream>

#include <vector>
#include <map>

#include "BitMapToKaoSwapCoreEngine.h"

using namespace std;

//----------------�ȉ��͓V�ċL�pKaoSwap���̕ϊ��K���̃R�A���̃R�A�B
//                ���������}���Ă���΁A������ł��ʂ̃c�[�������₷���B

/*
�V�ċL�ł́A��摜�͉��W�s�N�Z���łP�̒l�Ƃ��Ĉ����B
���W�s�N�Z���̂����A��ԉE�̃s�N�Z���ɂP���̐F�œh���Ă���ƈȉ��̒l�ƂȂ�
�������b�l�Ƃł���������B
�Â�����̃Q�[���ŁA�f�[�^�T�C�Y�����k���邽�߂ɁA���L�̂悤�ȑg�ݍ��킹�ɂ����(�r�b�g�V�t�g�ɑς���)�W�̊�b�J���[�����������̂ł��낤�B
���̂悤�ɂP�h���Ă���s�N�Z�������ւP�ړ����邽�тɁA�P�r�b�g�V�t�g�����B
[ ���������������� ] �́��P�������A�Ώۂ̐F�œh���Ă����ꍇ�̒l����b�l�B
[ ���������������� ] ���̂悤�ɂQ�ړ�����ƁA��b�l << 2 �Ƃ����悤�ɂQ�r�b�g�V�t�g���Z���邱�ƂƂȂ�B
*/
#define iKaoswapColorIndexBaseCnt	8
int KaoswapColorIndexBase[iKaoswapColorIndexBaseCnt] = {
	0x000000, // ��  R=0   G=0   B=0 
	0x000001, // ��	 R=17  G=68  B=153 
	0x000100, // ��	 R=187 G=68  B=0 
	0x000101, // ��	 R=204 G=136 B=68 
	0x010000, // ��	 R=0   G=119 B=51 
	0x010001, // ��	 R=85  G=153 B=170 
	0x010100, // ��	 R=255 G=187 B=85 
	0x010101, // ��  R=238 G=238 B=204 
};

/*
�W���ɕ��񂾃C���f�b�N�X�s�N�Z�������A�V�ċL��int�l�ɒ����B
INDEX�^�C�v��BMP�͖{���ɃJ���[INDEX���o�C�g�Ƃ��ĂP�������Ă邾���Ȃ̂Ŋy���ł���B
bmp�ł͈ȉ��̂悤�ɃC���f�b�N�X����[�������Ă���A
[ �O�Q�R�O�R�U�O�P ] �݂����ɂW�s�N�Z���ɑΉ�����J���[�C���f�b�N�X���P�������Ă��邾���B
(��256�F�p���b�g�ł́A���ۂɂ��̒ʂ�̃C���f�b�N�X�J���[��񂪃o�C�g���Ƃɓ����Ă���)
(��16 �F�p���b�g�ł́A���ꂪ���k����āA�P�o�C�g�ɂQ����[ 02 30 36 01 ]�݂����ɓ����Ă���̂ňȉ��̊֐��ɂ͂���𕪉����ēn������)

bmp�𕪉����A�����V�ċL�^�C�v�ւƕϊ�����B
�J���[�̃C���f�b�N�X����V�ċL�ɂ����Ă̊�b�l(���ColorIndexBase)�𓾂āA���Ԗڂ̃s�N�Z�����ɉ�����
Shift���Z����Ηǂ������ł���B

�V�ċL�ł�3�o�C�g���łP��ƂȂ�̂ŁA���^�[���l��int(4�o�C�g�S�������p�����킯�ł͂Ȃ�)
�ł��傫������(���g���G���f�B�A���Ń������z�u��ł͈�ԉE)�͏��00�B�v���ɁA
�Ԃ�lint = [xx xx xx 00]  xx xx xx �̕������V�ċL�̃f�[�^�Ƃ��Ďg����B
*/

#define iPixelPack	8

// iColorIndex[8] �� �W�̉��ɕ��񂾃s�N�Z���̏��A���g�͊e�X�̃C���f�b�N�X�J���[
int Cnv8BmpPixelTo8TSPixel(int iColorIndex[iPixelPack]) {
	// �W�̃s�N�Z���̃J���[�C���f�b�N�X�l�������Ă���
	int iReturn8TSPixcelValue = 0; // 0�̓f�t�H���g�̍��B�W�̉��ɂȂ�񂾃s�N�Z���������

	for (int ix = 0; ix<iPixelPack; ix++) {

		// �J���[�C���f�b�N�X�l�̃I�[�o�[�t���[�`�F�b�N
		int cIX = iColorIndex[ix];

		if (cIX >= iKaoswapColorIndexBaseCnt) {
			throw "ErrUsedColorPaletteOutOfRange!!"; // 

		}
		else {
			// �Y���̃J���[�C���f�b�N�X�l�̊�b�l*(7-Shift�l)
			// 8���񂾃s�N�Z�����̂����A��ԍ��Ȃ�7��b�l���r�b�g�V�t�g���ꂽ���̂ƂȂ�B
			iReturn8TSPixcelValue += KaoswapColorIndexBase[cIX] << (7 - ix);
		}

	}
	return iReturn8TSPixcelValue;
}




map<int, int> reverseHash;

// �V�ċL�̊�ȂǂɎg���Ă���W�s�N�Z���̃p�b�N���ꂽ����8��index���l��
// �������s�̉񐔂����ɑ����̂ŁAvector�Ȃǂ͗��p�����A���̂��P��int64�ōς�RESULT_8BMP_PIXEL�^�ɂ��Ă���B
RESULT_8BMP_PIXEL Cnv8TSPixelTo8BmpPixel(int iTsColorPackedIndex) {

	// �J���[�}�b�v���o���Ă��Ȃ���΁A�t�n�b�V�����\��
	if (reverseHash.size() == 0) {
		for (int i = 0; i < iKaoswapColorIndexBaseCnt; i++) {
			int value = KaoswapColorIndexBase[i];
			reverseHash[value] = i;
		}

	}

	// �W�̃s�N�Z���̃J���[�C���f�b�N�X�l�������Ă���
	RESULT_8BMP_PIXEL BmpColorIndexList;

	int bitFilter = KaoswapColorIndexBase[7]; // �Ō�̒l�� 0b [00000001][00000001][00000001] �Ƃ����t�B���^�[

	for (int ix = 0; ix < iPixelPack; ix++) {

		int bitShift = (7 - ix);
		int filter = bitFilter << bitShift;
		int iPackedIndex = iTsColorPackedIndex & filter;
		int iHashKey = iPackedIndex >> bitShift;

		BmpColorIndexList.at[ix] = reverseHash[iHashKey];
	}
	return BmpColorIndexList;
}
