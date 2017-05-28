Badgerotor
==========

Dies ist ein Rom für das [GPN17 Badge](https://entropia.de/GPN17:Badge), um
dies zusammen mit einer in der Hand haltbaren Richtantenne, zum Beispiel einer
[Arrow Antenne nach DL3RTL](http://www.dl3rtl.de/dl3rtl_arrow_antenna.php) als
Antennenrotor anzusprechen.

Das Badge wird dafür an der Antenne befestigt (Klebeband funktioniert, ggf.
geht Lego + Heißkleber auch gut, in Zukunft werde ich ich (oder Du?) mal was
mit 3D Druck dafür basteln). Nach dem Starten des Rom baut das Badge selber
ein Wlan auf, in das man mit dem Laptop mit rein geht. Darin kann es auf
168.168.4.1:4533 über das Hamlib Rotor Protokoll angesprochen werden. Als
Software auf dem Laptop bietet sich z.B.
[gpredict](http://gpredict.oz9aec.net/) an.

Das Passwort für das aufgebaute Wlan rotor-moho ist aktuell noch moho-rotor,
dies kann entweder jeder selber im Code anpassen oder in Zukunft wird es bald
Custom Wlans pro Badge geben. Z.B. könnte man automatisch das Config-Wlan
nutzen.

Mit den Leds wird intuitiv angezeigt, in welche Richtung das Badge / die
Antenne bewegt werden müssen, um die von der Steuerung angegebene Richtung
zu erreichen. Je heller die LEDs leuchten, desto größer ist die Abweichung.
Die aktuelle Position wird auch an die Steuerung zurückgemeldet.

Leider stört die Ansteuerung der LEDs relativ stark auf 70cm, in einem SDR ist
der Unterschied sehr gut sichtbar. Falls jemand eine Idee für eine Lösung
dieses Problems hat wäre das toll, ansonsten hilft es auch so genau zu zielen,
dass die LEDs aus gehen ;).

Zu beachten ist, dass gpredict es überhaupt nicht mag, wenn die TCP-Verbindung
zum Rotor kaputt geht. Vor dem Abschalten des Badges oder dem Ändern des Wlans
am Laptop also immer erst den Rotor in gpredict disengagen.

