# 🤖 Intelligenter KI-Mülleimer – HTL-Diplomarbeit 2025/26

Ein autonomes Mülltrennungssystem mit Mikrocontrollersteuerung (ESP32), KI-gestützter Bilderkennung (Raspberry Pi), Echtzeit-Sensorik und moderner Kommunikation über MQTT/HTTP. Entwickelt als Diplomarbeit an der HTL Anichstraße.
---

## 📍 Ausgangslage

In öffentlichen und Bürogebäuden wird Müll oft falsch getrennt. Trotz vorhandener Trennsysteme fehlt es den Nutzern häufig an Motivation oder Bewusstsein. Durch einen Mülleimer mit Objekterkennung, Sensorik und intelligenter Steuerung kann der Müll automatisch getrennt, Fehlwürfe reduziert und das Recycling effizienter gestaltet werden.
---

## 📌 Projektziel

Ziel dieses Projekts ist es, eine intelligente, wartungsarme Mülltonne zu entwickeln, die Müll automatisch erkennt, klassifiziert und in das passende Behälterfach sortiert. Das System kombiniert klassische Sensorik, moderne Steuerungstechnik und künstliche Intelligenz zur automatisierten Mülltrennung – ein Beitrag zu Umweltfreundlichkeit und Digitalisierung im Alltag.

---

## ⚙️ Systemübersicht

### 🧠 Hauptkomponenten

- **ESP32**: Steuert Sensoren, Motoren und die gesamte Logik
- **Raspberry Pi**: Führt die KI-gestützte Objekterkennung durch
- **Kameras**: Drei Kameras erfassen den Müll von mehreren Perspektiven
- **Sensoren**:
  - Ultraschallsensoren zur Füllstandmessung
  - Wägezelle zur Gewichtserkennung
  - NFC-Modul für Wartungsfreigabe
- **Aktoren**:
  - X/Y-Schlitten mit Schrittmotoren
  - Klappenmechanismus zur Müllfreigabe

---

## 🔄 Funktionsweise

1. **Erkennung**: Müllobjekt wird eingelegt und von drei Kameras erfasst.
2. **Analyse**: Bilder werden per JSON an die KI übermittelt (Raspberry Pi).
3. **Klassifikation**: Die KI erkennt die Müllart (z. B. Papier, Bio, PET).
4. **Sortierung**: Der ESP32 bewegt den Schlitten und öffnet das passende Fach.
5. **Überwachung**: Sensoren messen Gewicht und Füllstand in Echtzeit.
6. **Wartung**: Per NFC erhalten autorisierte Personen Zugriff zur Entleerung.
7. **Updates**: Firmware kann OTA (Over-the-Air) aktualisiert werden.
8. **Datenübertragung**: MQTT/HTTP zur Übermittlung an ein Frontend.

