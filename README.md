# I2C-ADC-DAC
Projektni zadatak se zasniva na radu sa pločicama ADC 12 Click i DAC 10 Click proizvođača Mikroelektronika. Date pločice je potrebno konfigurisati pomoću I2C protokola, tako da se omogući prikazivanje signala dovedenog na ulaz analogno/digitalnog konvertora na standardnom izlazu, te generisanje osnovnih tipova analognog signala: trougaonog, pravougaonog i sinusnog na izlazu digitalno/analognog konvertora.
## Konfiguracija Raspberry Pi platforme
Prije početka rada na projektnom zadatku potrebno je sprovesti niz koraka koji se odnose na konfiguraciju korištene Raspberry Pi platforme, tj. formatiranje SD kartice i preuzimanje i instalacija odgovarajućeg operativnog sistema. Nakon postavljanja korisničnog imena i hostname-a prilikom preuzimanja Raspberry Pi Imager-a, sljedeći korak jeste povezivanje sa Raspberry Pi platformom putem sljedeće komande:

``` ssh username@hostname.local ```

Nakon toga, potrebno je postaviti statičku IP adresu editovanjem fajla **/etc/dhcpcd.conf**. Detaljniji proces je objašnjen u dokumentaciji dostupnoj na kursu.

## Rad sa ADC 12 Click modulom
### Konfiguracija ADS7828 uređaja
ADC 12 Click je razvojna pločica na kojoj se nalazi ADS7828, 12-bitni uređaj koji posjeduje I2C interfejs i 8 kanala za analogne ulaze. Kako se on u kombinaciji sa Raspberry Pi platformom ponaša kao slave uređaj, prvo je potrebno odrediti njegovu I2C adresu. U ovom slučaju adresa ovog slave uređaja je sedmobitna, a najviših 5 bita je unaprijed podešeno na vrijednost 10010, dok su vrijednosti dva najniža bita A1 i A0 određene položajem SMD džampera, te u ovom slučaju imaju vrijednosti 00. Na osnovu toga, određena je adresa slave uređaja i ona iznosi **0x48**. Pored adresnog bajta, ADS7828 posjeduje i komandni bajt pomoću kog se ovaj uređaj konfiguriše. Za potrebe našeg projekta potrebno je omogućiti single-ended ulaz, na kanalu CH0, te uključiti A/D konvertor i njegovu internu referencu. Ovakvim izborom konfiguracije vrijednost komandnog bajta iznosi **0x8F**. Interna referenca, čija vrijednost napona iznosi 2.5 V, omogućava opseg napona na ulazu od 0 do 3.3 V, odnosno od 0 do napona napajanja uređaja. 

Kombinovanom I2C transakcijom se vrši konfiguracija ovog uređaja, tj. slanjem vrijednosti komandnog bajta, te čitanje dobijenih digitalnih vrijednosti. Dobijeni 12-bitni podaci, pakuju se u 2 bajta, s tim da prvo stiže MS bajt, a nakon njega i LS bajt. Na slici ispod prikazana je jedna kombinovana I2C transakcija, na kojoj se redom mogu uočiti sljedeći parametri: start sekvenca, sedmonitna adresa slave uređaja 1001000 (0x48), write pristup (0), te ACK od strane slave uređaja (0). Nakon toga se šalje komandni bajt 10001111 (0x8F) i dobijamo ACK (0). Zatim ide Repeated Start sekvenca, adresa slave uređaja 0x48, te Read pristup (1). Nakon toga slave uređaj ponovo šalje ACK (0), te dva bajta podataka. Detaljnija implementacija ove kombinovane I2C transakcije je dostupna u fajlu ADC/adc.c. Takođe, sa slike se vidi i brzina I2C transakcije i ona iznosi 50 kHz.

<p align="center">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116347913/27bf8b41-eee6-440a-9c56-f8d0fb609333)" width = "600", height = "400">


### Testiranje ADC 12 Click modula
Prije testiranja razvijenog programa potrebno je povezati ADC 12 Click modul sa odgovarajućim pinovima Rasberry Pi platforme. Na osnovu rasporeda pinova Raspberry Pi platforme dostupnog [ovde](https://pinout.xyz/), te [dokumentacije](https://download.mikroe.com/documents/datasheets/ADS7828_datasheet.pdf) ADC 12 Click modula, način povezivanja prikazan je u tabeli ispod:

| Raspberry Pi Pin | ADC 12 Click pin |
| :------: | :------: | 
| Power 3.3 V | 3.3 V | 
| GPIO 2 (I2C1 SDA) | SDA | 
| GPIO 3 (I2C1 SCL) | SCL | 
| Ground | GND |


Prvi vid testiranja ovog A/D konvertora je sproveden dovođenjem napona sa potenciometra na njegov ulaz (CH0). Rezultati testiranja prikazani su na slikama ispod, prilikom čega je upoređena vrijednost napona izmjerenog multimetrom i vrijednosti dobijene od A/D konvertora.

<p align="center">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116347913/07b5f59c-3045-4789-a12f-1ac2276130f9" width = "400", height = "600">

<p align="center">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116347913/6673f06d-424b-4a57-be63-f8813d24d06a" width = "400", height = "600">

<p align="center">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116347913/cef6ea10-9ec2-4fb6-aa84-fd6ae4da1154" width = "400", height = "600">

<p align="center">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116347913/c5f6fd85-725e-4e2d-9ea1-857a02d9d572" width = "400", height = "600">

Sirove podatke sa A/D konvertora potrebno je interpretirati kao stvarne, digitalne vrijednosti, koje su određene internom referencom i rezolucijom ovog uređaja. Kako je interna naponska referenca manja od napona napajanja, maksimalni napon koji je moguće prikazati biće 2.51 V. Sve vrijednosti napona na ulazu koje su veće ili jednake od interne reference, biće prikazane kao vrijednost od 2.51 V.

 <p align="center"> 
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116347913/14289d76-d2cb-413d-b04a-49a9ff1c0773" width = "400", height = "600">
   
Nakon testiranja pomoću potenciometra, na ulaz A/D konvertora doveden je i sinusni signal amplitude 1.65 V, ofseta 1.65V i frekvencije 1 kHz. Rezultat testiranja prikazan je na slici ispod:

 <p align="center"> 
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116347913/e0c7c5df-c01a-4fef-9925-2df0b5c608fe" width = "400", height = "600">

## Rad sa DAC 10 Click modulom
# Generisanje trougaonog signala
# Generisanje pravougaonog signala
# Generisanje sinusnog signala

## Replika ulaznog signala A/D konvertora na D/A konvertoru
U slučaju da se adrese ova dva slave uređaja razlikuju, što je moguće postići postavljanjem SMD jumper-a zaduženih za konfigurisanje njihovih I2C adresa u drugačije položaje od trenutnih, postoji mogućnost da se na izlazu D/A konvertora dobije replika signala koji je na ulazu A/D konvertora.
U tom smislu potrebno je inicirati kombinovanu I2C transkciju koja će prvo konfigurisati A/D konvertor i čitati podatke koje šalje, zatim transakciju koja će nam potvrditi da komuniciramo sa odgovarajućim uređajem čitanjem Device ID-a iz statusnog registra D/A konvertora. Pored toga potrebno je konfigurisati D/A konvertor upisivanjem odgovarajućih podataka u GENERAL_CONFIG i TRIGGER registre kao u prethodnim poglavljima, te dodatna transakcija koja će nam omogućiti upisivanje podataka dobijenih od A/D konvertora u DAC_DATA registar D/A konvertora.
Podaci koje ćemo dobiti od A/D konvertora potrebno je prilagoditi arhitekturi D/A konvertora, jer se radi o uređajima sa različitim rezolucijama. Pored toga, ovi uređaji imaju i različite referentne napone te je zbog toga neophodno izvršiti manipulaciju sirovih podataka sa A/D konvertora.
Sirove, 12-bitne podatke A/D konvertora ćemo prilagoditi D/A konvertoru tako što ih prvo podijelimo sa 4 (4096/1024 = 4), zatim ćemo ih pomnožiti sa referentnim naponom A/D konvertora (2.5 V) i podijeliti sa referentnim naponom D/A konvertora (3.3 V). Na ovaj način smo pronašli digitalni ekvivalent datog odmjerka signala. Na kraju, tako dobijene podatke potrebno je pomjeriti za dva bita ulijevo, kako bismo dobili odgovarajuće 12-bitno poravnanje koje D/A konvertor zahtjeva. Detaljnija implementacija ovog postupna dostupna je u fajlu **adc-dac.c**.





