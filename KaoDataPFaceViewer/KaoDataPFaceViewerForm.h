#pragma once;

#include "KaoDataStruct.h"

using namespace std;
using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::IO;

extern bool ImportKaoDataFileToProgramStruct(String^ strInputFileName);
extern Image^ GetBushouFaceImage(int iBushouID);
extern void ImportBmpToKaoData(char *szTargetFileName, int iBushouNum);


ref class KaoDataPFaceViewerForm : public Form {
public:
	static int BUSHOU_NUM = 0;

private:
	static List<PictureBox^>^ pbList;
	static List<Label^>^ lbList;
	static int iBushouStartPosition = 0;

	FileSystemWatcher^ watcher;

	static String^ strTargetFile;
	static String^ outDirName = "";

public:
	KaoDataPFaceViewerForm(String^ strTargetFileName) {
		this->KeyPreview = true; // �L�[�C�x���g�ɂ��ẮA�t�H�[���őS�Ď󂯎��

		iBushouStartPosition = 0;

		// �����^�[�Q�b�g���w�肳��Ă���Ȃ�΁c�A����ɂ���
		strTargetFile = strTargetFileName;
		outDirName = IO::Path::GetFileNameWithoutExtension(strTargetFile);

		this->Activated += gcnew EventHandler(this, &KaoDataPFaceViewerForm::form_Activated);
		this->Shown += gcnew EventHandler(this, &KaoDataPFaceViewerForm::form_Shown);

		SetFormAttribute();
	}
private:
	void form_Activated(Object^ o, EventArgs^ e) {
		ImportKaoDataFileToProgramStruct(strTargetFile);
	}

	void form_Shown(Object^ o, EventArgs^ e) {
		InitPictureBoxList();

		InitFileWatcher();
	}

	void SetFormAttribute() {
		this->StartPosition = ::FormStartPosition::Manual;
		this->Width = 840;
		this->Height = 1010;
		this->MaximizeBox = false;

		this->KeyDown += gcnew KeyEventHandler(this, &KaoDataPFaceViewerForm::form_KeyDown);
	}

	void InitPictureBoxList() {

		this->SuspendLayout();

		pbList = gcnew List<PictureBox^>();
		lbList = gcnew List<Label^>();

		int iBushouID = 0;
		for (int y = 0; y < 10; y++) {
			for (int x = 0; x < 10; x++) {
				PictureBox^ pb = gcnew PictureBox();
				pb->Width = FaceWidth;
				pb->Height = FaceHeight;
				pb->Left = x * 80 + 10;
				pb->Top = y * 96 + 10;
				pb->Image = GetBushouFaceImage(iBushouID);
				pb->Tag = iBushouID;
				pbList->Add(pb);
				this->Controls->Add(pb);

				Label^ lb = gcnew Label();
				lb->Width = FaceWidth;
				lb->Height = 13;
				lb->Left = pb->Left;
				lb->Top = pb->Bottom + 1;
				lb->Text = String::Format("{0:D4}", iBushouID + 1);
				lb->TextAlign = ContentAlignment::TopCenter;
				lbList->Add(lb);
				this->Controls->Add(lb);

				iBushouID++;
			}
		}

		this->ResumeLayout();
	}

	void UpdatePictureBoxList(int iStartBushouID) {

		this->SuspendLayout();

		int iBushouID = iStartBushouID;
		for (int i = 0; i < pbList->Count; i++) {

			PictureBox^ pb = pbList[i];
			pb->Image = GetBushouFaceImage(iBushouID);

			Label^ lb = lbList[i];
			lb->Text = String::Format("{0:D4}", iBushouID + 1);

			iBushouID++;
		}

		this->ResumeLayout();
	}

	void InitFileWatcher() {
		watcher = gcnew FileSystemWatcher();
		//�Ď�����f�B���N�g�����w��
		watcher->Path = Application::StartupPath;
		// ����̃t�@�C���݂̂̊Ď�
		watcher->Filter = IO::Path::GetFileName(strTargetFile);

		// �ŏI�������ݓ����̕ύX���Ď�
		watcher->NotifyFilter = NotifyFilters::LastWrite;

		//�T�u�f�B���N�g���͊Ď����Ȃ�
		watcher->IncludeSubdirectories = false;

		// UI�X���b�h�Ƀ}�[�V�������O
		watcher->SynchronizingObject = this;

		//�C�x���g�n���h���̒ǉ�
		watcher->Changed += gcnew FileSystemEventHandler(this, &KaoDataPFaceViewerForm::watcher_Changed);

		watcher->EnableRaisingEvents = true;

	}

	void form_KeyDown(Object^ sender, KeyEventArgs^ e) {

		int iPrevBushouStartPositon = iBushouStartPosition;

		int iFloor = (BUSHOU_NUM / 100) * 100;

		auto code = e->KeyCode;
		// �O�̃f�[�^�Ƃ����悤�ȈӖ����w���L�[�{�[�h��������
		if (code == Keys::Left || code == Keys::Up || code == Keys::PageUp) {
			iBushouStartPosition -= 100;
		}
		// ���̃f�[�^�Ƃ����悤�ȈӖ����w���L�[�{�[�h��������
		else if (code == Keys::Right || code == Keys::Down || code == Keys::PageDown) {
			iBushouStartPosition += 100;
		}

		else if (code == Keys::Home) {
			iBushouStartPosition = 0;
		}
		else if (code == Keys::End) {
			iBushouStartPosition = iFloor; // 100 �Ŋ����ė]�������͐؂�̂�
		}

		if (iBushouStartPosition < 0) {
			iBushouStartPosition = 0;
		}
		if (iBushouStartPosition > iFloor) {
			iBushouStartPosition = iFloor;
		}

		if (code == Keys::O) {
			auto ret = MessageBox::Show("�S�Ă̊�� " + outDirName + " �f�B���N�g���ւƏo�͂��܂����H", "�m�F", ::MessageBoxButtons::YesNo);
			if (ret == ::DialogResult::Yes) {
				if (!Directory::Exists(outDirName)) {
					Directory::CreateDirectory(outDirName);
				}

				OutputAllFaceToBmp();
				MessageBox::Show("����");
			}
		}

		else if (code == Keys::I) {
			auto ret = MessageBox::Show("�S�Ă̊�� " + outDirName + " �f�B���N�g������ " + strTargetFile + " �ւƎ捞���܂����H", "�m�F", ::MessageBoxButtons::YesNo);
			if (ret == ::DialogResult::Yes) {
				char szTargetFile[512] = "";
				wsprintf(szTargetFile, "%s", strTargetFile);
				ImportBmpToKaoData(szTargetFile, BUSHOU_NUM);
			}
		}

		if (iPrevBushouStartPositon != iBushouStartPosition) {
			UpdatePictureBoxList(iBushouStartPosition);
		}
	}

	void OutputAllFaceToBmp() {
		int error_cnt = 0;
		for (int b = 0; b < BUSHOU_NUM; b++) {
			Image^ img = GetBushouFaceImage(b);
			String^ filefullpath = String::Format(outDirName + "\\{0:D4}.bmp", b + 1); // �t�@�C���ɂ��鎞�́A�P�������l
			try {
				img->Save(filefullpath);
			}
			catch (Exception^ e) {
				error_cnt++;
				MessageBox::Show(e->Message + "\n" + filefullpath);
			}
		}
	}

	void watcher_Changed(Object^ sender, FileSystemEventArgs ^ e) {
		System::Diagnostics::Trace::WriteLine(e->Name);
		// ��f�[�^�S�̂�ǂݒ���
		ImportKaoDataFileToProgramStruct(strTargetFile);
		// �\���X�V
		UpdatePictureBoxList(iBushouStartPosition);
	}

};
