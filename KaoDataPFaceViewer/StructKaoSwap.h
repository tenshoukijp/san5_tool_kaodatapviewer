#pragma once

#pragma pack(1)

#include <windows.h>

//----------------�ȉ��͓V�ċL�pKaoSwap���̉�f���

// ��1�񂪏c��80�Ŋ�摜���o���オ��B
struct KAOSWAP_TENSHOUKI_FACE_64x80
{
	BYTE data[1920]; // 1920�o�C�g����P�B�P���C��(64�s�N�Z��)��24�o�C�g�ł��邪�A���̊Ǘ��͓��ɕs�v�B
};



template <int BUSHOU_NUM>	// 1332=�ʏ펞�̕����̐��B1932��2932�l���ȂǗp�ɕύX�ł���悤�e���v���[�g��
struct KAOSWAP_TENSHOUKI
{
	KAOSWAP_TENSHOUKI_FACE_64x80 face[BUSHOU_NUM];	// ��摜���e�B�l����
};

