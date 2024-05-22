#define WIN32_LEAN_AND_MEAN // макрос

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData;  
    ADDRINFO hints;  
    ADDRINFO* addrResult;  
    SOCKET ListenSocket = INVALID_SOCKET;  // Сокет для прослушивания входящих соединений
    SOCKET ConnectSocket = INVALID_SOCKET;  // Сокет для соединения с клиентом
    char recvBuffer[512];  // Буфер для получения данных

    const char* sendBuffer = "Hello from server";  // Сообщение для отправки клиенту


    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;  // Использование IPv4
    hints.ai_socktype = SOCK_STREAM;  
    hints.ai_protocol = IPPROTO_TCP;  // Протокол TCP
    hints.ai_flags = AI_PASSIVE;  

    // Получение адресной информации для создания сокета
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Создание сокета для прослушивания
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Привязка сокета к адресу
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Bind failed, error: " << result << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Начало прослушивания входящих соединений
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cout << "Listen failed, error: " << result << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Принятие входящего соединения
    ConnectSocket = accept(ListenSocket, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Accept failed, error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Закрытие сокета для прослушивания
    closesocket(ListenSocket);

    // Получение и отправка данных
    do {
        ZeroMemory(recvBuffer, 512);  // Очистка буфера для получения данных
        result = recv(ConnectSocket, recvBuffer, 512, 0);  // Получение данных из сокета
        if (result > 0) { 
            cout << "Received " << result << " bytes" << endl;  
            cout << "Received data: " << recvBuffer << endl;  

            // Отправка данных клиенту
            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {  
                cout << "Send failed, error: " << result << endl;
                closesocket(ConnectSocket);
                freeaddrinfo(addrResult);
                WSACleanup();
                return 1;
            }
        }
        else if (result == 0) {  
            cout << "Connection closing" << endl;
        }
        else {  
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            freeaddrinfo(addrResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);  

    // Завершение отправки данных клиенту
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) { 
        cout << "Shutdown failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Закрытие сокета для соединения с клиентом и освобождение памяти
    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup(); 
    return 0;  
}