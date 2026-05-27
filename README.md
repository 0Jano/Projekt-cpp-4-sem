# SyncCal — współdzielony kalendarz desktopowy

**SyncCal** to aplikacja desktopowa napisana w C++ z wykorzystaniem frameworka Qt 6, umożliwiająca zarządzanie i współdzielenie kalendarzy.  
Projekt powstał jako realizacja projektu semestralnego z przedmiotu Projekt C++.

---

## Cel projektu

SyncCal ma na celu uproszczenie zarządzania czasem i wydarzeniami poprzez:
- tworzenie osobistych i współdzielonych kalendarzy
- zarządzanie wydarzeniami (dodawanie, edycja, usuwanie)
- przejrzyste widoki (miesięczny, tygodniowy)
- integrację z lokalną bazą danych w celu trwałego przechowywania danych

---

## Technologie

Projekt wykorzystuje nowoczesne standardy i biblioteki:

- **Język:** C++17
- **Framework:** Qt 6 (Widgets, Sql, Core)
- **System budowania:** CMake
- **Baza danych:** SQLite (za pośrednictwem modułu Qt SQL)

---

## Architektura projektu

Kod źródłowy jest podzielony na logiczne warstwy:

- `src/models/` — struktury danych (użytkownik, wydarzenie, kalendarz)
- `src/managers/` — logika biznesowa i operacje na bazie danych (`AuthManager`, `DatabaseManager`, `EventManager`, `CalendarManager`)
- `src/ui/` — komponenty interfejsu użytkownika i dialogi
- `tests/` — testy jednostkowe (w trakcie rozwoju)

---

## Harmonogram prac

Aplikacja jest rozwijana zgodnie z poniższym planem (T = tydzień):

- [x] **T1:** Analiza wymagań, repozytorium, konfiguracja CMake i podstawowa struktura
- [x] **T2:** Projekt architektury i diagramów klas (User, Event, Calendar, Database)
- [x] **T3:** Klasa User oraz system logowania i rejestracji
- [x] **T4:** Klasa Event i podstawowa obsługa wydarzeń
- [x] **T5:** Klasa Calendar, operacje CRUD oraz podstawowe widoki w pamięci
- [x] **T6:** Integracja z Qt SQL i trwałość danych (SQLite)
- [x] **T7:** GUI: Widok miesiąca, tygodnia oraz okna dialogowe dodawania wydarzeń
- [x] **T8:** Współdzielenie kalendarzy i zarządzanie rolami (właściciel/uczestnik)
- [x] **T9:** Wykrywanie konfliktów, import/eksport .ics oraz testy jednostkowe (w trakcie rozwijania)
- [ ] **T10:** Testy końcowe, dokumentacja i przygotowanie prezentacji

---

## Uruchomienie projektu

### Wymagania
- CMake (>= 3.16)
- Kompilator obsługujący C++17 (np. GCC, MSVC, Clang)
- Qt 6 (moduły: Widgets, Sql, Core)

### Budowanie i uruchomienie
```bash
mkdir build
cd build
cmake ..
make
./SyncCal
```

---

## Autor
**Jan Grabarz**

---

## Repozytorium
[https://github.com/0Jano/Projekt-cpp-4-sem](https://github.com/0Jano/Projekt-cpp-4-sem)
