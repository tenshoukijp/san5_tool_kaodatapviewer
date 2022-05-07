#pragma once

#include <windows.h>

#pragma pack(1)
/*
	�\���̂Ƃ��ŏ���Ɏw��̃r�b�g�Ƃ͂��ƂȂ�悤�Ƀp�b�L���O���Đ��񂵂Ȃ������Ƃ̖h�~�B
	#pragma pack(1)�Ƃ��邱�ƂŁA1bit���̐���ƂȂ邽�߁A������A���񂳂�Ȃ��Ȃ�B
*/


/*
�ȉ��̓r�b�g�}�b�v�w�b�_�[

	�I�t�Z�b�g	�o�C�g��	�\���̃t�B�[���h��	����
	0			2			BfType				�t�@�C���̃^�C�v�A�r�b�g�}�b�v������BM�i0x424d�j������B
	2			4			BfSize				�t�@�C���̃o�C�g���B�t�@�C���̃T�C�Y�̏�񂪓���B
	6			2			BfReserved1			�\���B�l�́A0x00������B
	8			2			BfReserved2			�\���B�l�́A0x00������B

	�`�`�����܂ł͂͂����肢���Ăǂ��ł������`�`


	�������d�v�Ƃ������A�摜���e�̊J�n�_�`�`

	000A		4			BfOffBits			�t�@�C���̐擪����A�摜���܂ł̃I�t�Z�b�g�B�擪����Ȃ̂ŁA������A���̃A�h���X����摜���e���n�܂��Ă���B
	���߂����ɂ��Ȃ��̂́A8�p���b�g��8�p���b�g�g�p��bmp�t�@�C����16�p���b�g��8�p���b�g�g�p��bmp�t�@�C���Ƃł͔����Ƀw�b�_�T�C�Y���قȂ邽�߁B
	�_�p�b�`�̊�c�[���͂����̔��f�������Ă��āA���񂵂��ɂ����B

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
	�I�t�Z�b�g	�o�C�g��	�\���̃t�B�[���h��	����
	000E		4			BiSize				BITMAPINFO HEADER�\���̂̃T�C�Y�B
	12			4			BiWidth				�摜�̕��B�P�ʂ̓s�N�Z���B
	16			4			BiHeight			�摜�̍����B�P�ʂ̓s�N�Z���B

struct BITMAPINFOHEADER;
{
	DWORD  biSize; 
	LONG   biWidth; 
	LONG   biHeight; 
	�`�`�������牺�̗v�f�̓o�[�W�����ɂ���ĈقȂ�
};
*/

//----------------�ȏオ�ėp��BitMap�t�H�[�}�b�g




//----------------�ȉ��͔ėpBitMap�W�F�̓V�ċL��(�Ƃ������A�p���b�g���W���P�U���A�擪�W�F�𗘗p����64*80�̃T�C�Y��BMP)

/*
	�ȉ��R�Ŋ����舵���₷�����邽�߂̃Z�b�g
	���̂悤�ɂ���̂́Abmp�t�@�C���͉摜�̍����̏�񂩂�A�P���C������ւƌ������ď�񂪊i�[����Ă���̂ɑ΂��āA
	�V�ċL�ł́A���ォ���񂪊i�[����Ă��邽�߁B
	�|�C���^�v�Z���Ă��ǂ����A��X���₷�����邽�߁A���A�����������҂̕ϊ��v�Z�������Ƃ��邽�߁A���̂悤�ɂ���B
	(�V�ċL�̊�摜�̃t�H�[�}�b�g��64x80�Œ�Ō����ĕς��Ȃ����߁A�����^�C���v�Z�͖��ʂ������Ƃ������Ƃ�����)
*/

/*
	�v�Z�Ɏg���ŏ��P�ʂ�1�s�N�Z���̏��4bit�ɓ����Ă��āA8�s�N�Z����4�o�C�g
	���ꂪ1�p�b�N
	���L�\���̂ŁA�P�P�̃r�b�g�t�B�[���h�ɖ��O�t���Ȃ̂́A
	�L�q�I�ɂ͑������ɂȂ邪�A���g���G���f�B�A���ɂ��Ђ�����Ԃ����A���낢�날��̂ŁA
	�v�Z�ʓI�ɂ́A�_�C�i�~�b�N�Ɍv�Z��������ǂ�
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

// �オ�W�p�b�N�Ŋ�摜�̉�1��ƂȂ�B
struct BIT64PIXEL_LINE
{
	BIT8PIXEL_PACK	bit8pixel_pack[8]; // 8Pixel ��8��64Pixel �P���C����64�o�C�g
};

#define FACE_LINE_NUM		80

// ��1�񂪏c��80�Ŋ�摜���o���オ��B
struct BITMAP_TENSHOUKI_FACE
{
	BIT64PIXEL_LINE bit64pixel_line[FACE_LINE_NUM]; // �c��80line���݂���B
};


