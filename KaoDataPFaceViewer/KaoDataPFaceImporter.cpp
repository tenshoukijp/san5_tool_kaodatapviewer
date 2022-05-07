#include <shlwapi.h>
#include "BitMapToKaoSwap.h"
#include <string>

#pragma comment(lib, "shlwapi")

using namespace std;

template <int TNUM>
void doKaoswapImport(char *szTargetKaoSwapName) {
	auto *pBmpToKS = new BitMapToKaoSwapManager<TNUM>(szTargetKaoSwapName);
	int Err = pBmpToKS->ReadKaoSwap();
	if (!Err) {
		try {
			for (int i = 1; i <= TNUM; i++) {
				pBmpToKS->Execute(i);
			}
			pBmpToKS->WriteKaoSwap();
		}
		catch (...) { }
	}

	delete pBmpToKS;
}

void ImportBmpToKaoData(char *szTargetFileName, int iBushouNum) {

	// このソースはひど過ぎないかい…
	if (iBushouNum == 783) {
		doKaoswapImport<783>(szTargetFileName);
	}
	else if (iBushouNum == 382) {
		doKaoswapImport<382>(szTargetFileName);
	}
	// ちょっとどうなのこれは…というコードだが、C++14でもない限り、変数をテンプレートにはできない。
}

