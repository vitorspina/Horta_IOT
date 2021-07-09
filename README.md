# Horta_IOT
Este projeto consiste em uma horta controlada por um ESP 32 em conjunto com alguns sensores, uma bomba de aquário e um display OLED responsável por criar um dashboard com os dados sobre a horta. Juntamente com o esp foi utilizado um bot no telegram escrito em python 3, para se mater o controle de acesso as informações da horta, foi criado um banco de dados mySQL com os ids dos responsaveis. Os sensores utilizados foram:
* Sensor de temperatura DS18b20;
* Sensor de Umidade e temperatura DHT 11;
* Um módulo relé para controlar a bomba;
* LDR para medir a intesidade do sol;
* Um módulo de umidade do solo LM393;
* Display OLED SSD1306 128x64;
O esquematico está disposto abaixo e o firmware se encontra na pasta arduino com o nome firmware_final.ino.
(Images/schematic.pdf)