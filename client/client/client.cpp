#define WIN32_LEAN_AND_MEAN //макрос

#include <Windows.h>  
#include <iostream>  
#include <WinSock2.h>  
#include <WS2tcpip.h> 

#pragma comment(lib, "Ws2_32.lib")  

using namespace std;  

int main() {
    WSADATA wsaData;  
    ADDRINFO hints;  
    ADDRINFO* addrResult = nullptr;  
    SOCKET ConnectSocket = INVALID_SOCKET;  // Сокет для подключения к серверу
    char recvBuffer[512];  // Буфер для получения данных от сервера

    const char* sendBuffer1 = "Hello from client 1";  // Первое сообщение для отправки серверу
    const char* sendBuffer2 = "Hello from client 2";  // Второе сообщение для отправки серверу

    // Инициализация библиотеки Windows Sockets
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;  // Использование IPv4
    hints.ai_socktype = SOCK_STREAM;  // Использование TCP
    hints.ai_protocol = IPPROTO_TCP;  // Использование протокола TCP

    // Получение адресной информации для подключения к "localhost" на порту "666"
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Создание сокета на основе полученной информации
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Отправка первого сообщения серверу
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);  // Функция send отправляет данные в сокет
    if (result == SOCKET_ERROR) {  // Проверка на ошибку отправки
        cout << "Send failed, error: " << result << endl;  // Вывод сообщения об ошибке
        closesocket(ConnectSocket);  // Закрытие сокета
        freeaddrinfo(addrResult);  // Освобождение памяти
        WSACleanup();
        return 1;
        cout << "Sent: " << result << " bytes" << endl;


    }
    // Отправка второго сообщения
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    // Завершение отправки данных на сервер
    result = shutdown(ConnectSocket, SD_SEND);  // Функция shutdown завершает передачу данных по сокету
    if (result == SOCKET_ERROR) {  // Проверка на ошибку завершения отправки данных
        cout << "Shutdown failed, error: " << result << endl;  // Вывод сообщения об ошибке
        closesocket(ConnectSocket);  // Закрытие сокета
        freeaddrinfo(addrResult);  // Освобождение памяти
        WSACleanup(); 
        return 1; 
    }

    // Получение данных от сервера
    do {
        ZeroMemory(recvBuffer, 512);  // Очистка буфера для получения данных
        result = recv(ConnectSocket, recvBuffer, 512, 0);  // Получение данных из сокета
        if (result > 0) {  
            cout << "Received " << result << " bytes" << endl; 
            cout << "Received data: " << recvBuffer << endl; 
        }
        else if (result == 0) {  
            cout << "Connection closed" << endl;  
        }
        else {  
            cout << "Recv failed, error: " << WSAGetLastError() << endl;  
        }
    } while (result > 0); 

    // Закрытие сокета и освобождение ресурсов
    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}
