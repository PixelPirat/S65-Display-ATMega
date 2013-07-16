Siemens S65, M65, CX65 und SK65 Display - ATMega
------------------------------------------------
  
Die Bibliothek beinhaltet die grundlegenden Funktionen um Displays mit der Seriennummer LS020xxx oder LQ020xxx, die in den Mobiltelefonen Siemens S65 / M65 / CX65 / SK65 verbaut worden sind, anzusprechen.

Grundlage für diese Bibliothek ist die Vorarbeit von [Christian Kranz](http://www.superkranz.de/christian/S65_Display/DisplayIndex.html "Christian Kranz - Homepage").
  
---  

Getestet mit `ATMega8`, `ATMega168` und `ATMega328`.  
Mit _sehr hoher Wahrscheinlichkeit_ auf einem _Arduino_ lauffähig. (Mikrocontroller sind dieselben) 
  
[Demobild](https://raw.github.com/PixelPirat/S65-Display-ATMega/master/Bilder/s65display_demo.png "Demobild")  
  
[Schaltplan](https://raw.github.com/PixelPirat/S65-Display-ATMega/master/Bilder/s65display_schaltplan.jpg "Schaltplan")
  
*Anwendungsbeispiel und ~~Schaltplan~~ folgen ...*
  
---
  
**Display Spezifikationen**
  * 132 x 176 Pixel
  * 65.536 Farben                       (8 oder 16 Bit)
  * SPI Ansteuerung
  * Hintergrundlicht                    (Helligkeit per PWM einstellbar)
  * 38,20mm x 55,80mm Displaygröße

---

**Bibliothek beinhaltet**
  * Bildschirm löschen
  * Pixel setzen
  * Linie zeichnen                      (Bresenham-Algorithmus)
  * Rechteck zeichnen
  * Kreis / Ellipse zeichnen            (Bresenham-Algorithmus)
  * Schriftfarbe
  * Hintergrundfarbe
  * Schriftgröße                        (Normal und Groß - Abhängig von der Fonttabelle)
  * Shutdown des Displays
  * `printf()` auf das Display
  * Horizontale oder Vertikale Ausgabe
  * Ausgabe einer Monochromen Bitmap

---

Kritik, Anregungen oder Bugs können an `grahlluc@gmail.com` gesendet werden.
