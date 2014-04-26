
#include <Windows.h>
#include <process.h>
#include <conio.h>
#include <stdio.h>

int main(void) {

	HANDLE ntleasig = CreateEventW(NULL, FALSE, FALSE, L"ntleasig");
	fprintf(stderr, "enter <ESCAPE> for exit or <RETURN> for trigger.\n");

	while (ntleasig/*true?*/) {
		if (_kbhit()) {
			int ch = _getch();
			switch (ch) {
				// ESC: 
			case 0x1B: CloseHandle(ntleasig); ntleasig = NULL;
				fprintf(stderr, "ESCAPE : signal exit!\n");
				break;
				// RETURN:
			case 0x0D: SetEvent(ntleasig); // 
				fprintf(stderr, "RETURN : signal is triggered!\n");
				break;
			default: 
				fprintf(stderr, "unknown key = 0x%02X, please input 'ESCAPE' or 'RETURN' \n", ch);
				break;
			}
		}
		Sleep(50);
	}
	// exit ... 
	return (0);
}
