# RPI-GUI
...eine Benutzeroberflaeche fuer meinen Rasberry PI...

## Autor:
Uwe Berger; bergeruw(at)gmx(dot)net; 2014, 2015 
 
## Projektseite:
http://www.bralug.de/wiki/Raspberry_Pi_als_Musikabspielger%C3%A4t_an_der_Stereoanlage

## Hardware:
siehe Projektseite...

## Installation:
* Installation dev-Pakete zu diversen Libs
  * libgen
  * tslib
  * libmpd
  * libconfig
  * libjpeg
  * ... ;-)
* Download rpi_gui-Quellen:
```
git clone git://github.com/boerge42/rpi_gui.git
```
* Übersetzung und Installation:
```
make 
sudo make install
sudo cp rpi_gui.conf /etc/rpi_gui.conf
cp jpeg/*.* /wohin_auch_immer/
```
* Einrichten Generierung Wettervorhersagedatei (Bsp. weather_forecast.conf)
  z.B. Konvertierungsscript zyklisch ueber Cronjob... 
  (Bsp. script/weather_tuxnet24.tcl)
* Einrichten Generierung aktuelle Wetterwerte (Bsp. weather_current.conf)
  z.B. Konvertierungsscript zyklisch ueber Cronjob... 
  (Bsp. script/weather_current.tcl)
* Konfigurationsdatei /etc/rpi_gui.conf entsprechend anpassen
* Oberfläche starten (siehe auch Projektseite...):
```
sudo rpi_gui
```

## Funktionalitaet:
* Anzeige/Bedienung ueber Touch-Display
* Anzeige der aktuellen Werte aus meiner Wetter-Datenbank (ueber eine
  Dateischnittstelle; siehe oben)
  http://www.bralug.de/wiki/Wetterdaten_mit_Linux_aufzeichnen_und_verarbeiten
* Wettervorhersage
  (Daten von http://weather.tuxnet24.de)
* rudimentäre Steuerung Music Player Daemon (MPD)
* Anzeige diverser Infos zur laufenden mp3-Datei
* ein paar Systeminformationen zum Raspberry

[![Screen-Music](https://github.com/boerge42/rpi_gui/blob/master/screens/rpi_gui_music.png)]
[![Screen-Queue](https://github.com/boerge42/rpi_gui/blob/master/screens/rpi_gui_queue.png)]
[![Screen-Wetter](https://github.com/boerge42/rpi_gui/blob/master/screens/rpi_gui_wetter.png)]
[![Screen-System](https://github.com/boerge42/rpi_gui/blob/master/screens/rpi_gui_system.png)]


---------
Have fun!
