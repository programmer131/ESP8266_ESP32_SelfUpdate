repo contains Arduino sketch that include self OTA support.

ESP32 OTA Demo: https://youtu.be/qCmdUtguwPw

demonstration and code explanation https://youtu.be/FuY6BobS-1k

Just another small demo:  https://youtu.be/2J8KK5Cxf6g

Update: removed fingerprint and added rootca certificate for esp8266 but bin file downloading
is quite slow now.

June 2022 update: Instead of using static url in your firmware, just send it in mqtt message, this way you can avoid scenarios where url/domain goes offline. This project utilize this technique, https://github.com/programmer131/cam-flood-light
