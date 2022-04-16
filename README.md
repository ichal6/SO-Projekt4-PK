# SO-Projekt4-PK
The fourth projekt from subject "Systemy Operacyjne" from Information Technologies studies 

## PROJEKT NR 4 – KOMUNIKACJA TYPU ŁĄCZE KOMUNIKACYJNE

Projekt składa się z dwóch programów uruchamianych niezależnie: serwer i klient.
Proces klient wysyła do procesu serwera ciąg znaków. Serwer odbiera ten ciąg znaków i
przetwarza go zmieniając w nim wszystkie litery na duże, a następnie wysyła tak przetworzony
ciąg znaków z powrotem do klienta. Klient odbiera przetworzony ciąg znaków i wypisuje go na
ekranie. Posługując się mechanizmem kolejki komunikatów, należy zaimplementować powyższe
zadanie typu klient-serwer z możliwością obsługi wielu klientów jednocześnie. W rozwiązaniu
użyć jednej kolejki komunikatów. Zastosować odpowiednie etykietowanie komunikatów w celu
rozróżniania w kolejce danych dla serwera oraz danych dla poszczególnych klientów.


Serwer tworzy kolejkę komunikatów i oczekuje na komunikaty od klientów. Serwer usuwa
kolejkę po otrzymaniu sygnału zdefiniowanego przez użytkownika (np. SIGINT).
Klient to aplikacja wielowątkowa (patrz projekt nr 2). Za wysyłanie komunikatów odpowiada
watek 1, za odbieranie wątek2. Watki działają asynchronicznie tzn. wątek 1 może wysłać kilka
komunikatów zanim wątek 2 odbierze jakikolwiek komunikach zwrotny od serwera. Należy
obsłużyć błąd przepełnienia kolejki oraz przepełnienia komunikatu.
Dla funkcji systemowych zaprogramować obsługę błędów w oparciu o funkcję perror() i zmienną
errno
