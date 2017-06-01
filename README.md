Badgerotor
==========

Dies ist ein Rom für das [GPN17 Badge](https://entropia.de/GPN17:Badge), um
dies zusammen mit einer in der Hand haltbaren Richtantenne, zum Beispiel einer
[Arrow Antenne nach DL3RTL](http://www.dl3rtl.de/dl3rtl_arrow_antenna.php) als
Antennenrotor anzusprechen.

Das Badge wird dafür an der Antenne befestigt (Klebeband funktioniert, ggf.
geht Lego + Heißkleber auch gut, in Zukunft werde ich ich (oder Du?) mal was
mit 3D Druck dafür basteln). Nach dem Starten des Rom baut das Badge ein WLAN
auf oder verbindet sich zu einem WLAN. Darin kann es auf
Port 4533 über das Hamlib Rotor Protokoll angesprochen werden. Als
Software auf dem Laptop bietet sich z.B.
[gpredict](http://gpredict.oz9aec.net/) an.

Falls beim Starten der Joystick betätigt wird verbindet sich das Badge zum
Configwlan, ansonsten baut es selber das WLAN badge-rotor auf.

Mit den Leds wird intuitiv angezeigt, in welche Richtung das Badge / die
Antenne bewegt werden müssen, um die von der Steuerung angegebene Richtung
zu erreichen. Je heller die LEDs leuchten, desto größer ist die Abweichung.
Die aktuelle Position wird auch an die Steuerung zurückgemeldet.

Auf dem Display wird eine Polardarstellung des aktuellen und der gewünschten
Winkels angezeigt.

Leider stört die Ansteuerung der LEDs relativ stark auf 70cm, in einem SDR ist
der Unterschied sehr gut sichtbar. Falls jemand eine Idee für eine Lösung
dieses Problems hat wäre das toll, ansonsten hilft es auch so genau zu zielen,
dass die LEDs aus gehen ;).

Zu beachten ist, dass gpredict es überhaupt nicht mag, wenn die TCP-Verbindung
zum Rotor kaputt geht. Vor dem Abschalten des Badges oder dem Ändern des Wlans
am Laptop also immer erst den Rotor in gpredict disengagen.

