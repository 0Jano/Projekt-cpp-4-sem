# SyncCal — współdzielony kalendarz desktopowy

**SyncCal** to aplikacja desktopowa napisana w C++, umożliwiająca współdzielenie kalendarzy w grupie użytkowników.  
Projekt powstał jako realizacja projektu semestralnego z przedmiotu Projekt C++.

---

## Cel projektu

Wspólne umawianie terminów w zespołach często jest chaotyczne i nieefektywne.  
SyncCal rozwiązuje ten problem poprzez:

- możliwość tworzenia kalendarzy osobistych i grupowych  
- udostępnianie wydarzeń innym użytkownikom  
- przegląd wydarzeń w różnych widokach  
- zarządzanie rolami i uprawnieniami  

---

## Zakres MVP

W pierwszej wersji aplikacja oferuje:

- rejestrację i logowanie użytkowników  
- tworzenie kalendarzy (prywatncyh i grupowych)  
- dodawanie, edycję i usuwanie wydarzeń (CRUD)  
- udostępnianie wydarzeń i zapraszanie uczestników  
- widok kalendarza:
  - miesięczny  
  - tygodniowy  
  - lista wydarzeń  
- role użytkowników (właściciel / uczestnik)

---

## Planowane rozszerzenia

- wykrywanie konfliktów terminów  
- sugestie wolnych okien czasowych  
- moduł list To‑Do powiązanych z wydarzeniami  
- eksport / import kalendarza (np. pliki .ics)  
- testy jednostkowe  
- integracja CI/CD  

---

## Technologie

Projekt wykorzystuje:

- C++17 / C++20  
- Qt 6 — interfejs graficzny  
- CMake — system budowania  
- MySQL — lokalna baza danych  
- nlohmann/json — obsługa JSON  
- libcurl / cpp-httplib — komunikacja HTTP  
- spdlog — logowanie  

Testy:

- GoogleTest lub Catch2  

---

## Architektura

Główne klasy:

- User — reprezentuje użytkownika  
- Event — wydarzenie w kalendarzu  
- Calendar — zarządzanie wydarzeniami  
- Database — komunikacja z bazą danych  

Model działania:

Desktop client + współdzielona baza danych

---

## Interfejs użytkownika

Planowane widoki:

- widok miesiąca  
- widok tygodnia  
- widok listy wydarzeń

Funkcje UI:

- panel kalendarzy  
- filtrowanie wydarzeń  
- formularz dodawania wydarzenia  
- lista uczestników  

---

## Uruchomienie projektu

### Wymagania

- CMake  
- kompilator C++  
- Qt 6  
- MySQL  

### Budowanie



---

## Harmonogram

1. analiza i struktura projektu  
2. projekt architektury  
3. system użytkowników  
4. obsługa wydarzeń  
5. zarządzanie kalendarzem  
6. integracja z bazą danych  
7. implementacja GUI  
8. udostępnianie i role  
9. konflikty terminów i testy  
10. poprawki i stabilizacja  

---

## Autor

Jan Grabarz

---

## Repozytorium

https://github.com/0Jano/Projekt-cpp-4-sem
