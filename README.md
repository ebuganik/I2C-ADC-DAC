# I2C-ADC-DAC
Projektni zadatak se zasniva na radu sa pločicama ADC 12 Click i DAC 10 Click proizvođača Mikroelektronika. Date pločice je potrebno konfigurisati pomoću I2C protokola, tako da se omogući prikazivanje signala dovedenog na ulaz analogno/digitalnog konvertora na standardnom izlazu, te generisanje osnovnih tipova analognih signala: trougaonog, pravougaonog i sinusnog na izlazu digitalno/analognog konvertora.
## Konfiguracija Raspberry Pi platforme
Prije početka rada na projektnom zadatku potrebno je sprovesti niz koraka koji se odnose na konfiguraciju korištene Raspberry Pi platforme, tj. formatiranje SD kartice i preuzimanje i instalacija odgovarajućeg operativnog sistema. Nakon postavljanja korisničnog imena i hostname-a prilikom preuzimanja Raspberry Pi Imager-a, sljedeći korak jeste povezivanje sa Raspberry Pi platformom putem sljedeće komande:

``` ssh username@hostname.local ```

Nakon toga, potrebno je postaviti statičku IP adresu editovanjem fajla **/etc/dhcpcd.conf**. Detaljniji proces je objašnjen u dokumentaciji dostupnoj na kursu.

## Rad sa ADC 12 Click modulom
### Konfiguracija ADS7828 uređaja
ADC 12 Click je razvojna pločica na kojoj se nalazi ADS7828, 12-bitni uređaj koji posjeduje I2C interfejs i 8 kanala za analogne ulaze. Kako se on u kombinaciji sa Raspberry Pi platformom ponaša kao slave uređaj, prvo je potrebno odrediti njegovu I2C adresu. U ovom slučaju adresa ovog slave uređaja je sedmobitna, a najviših 5 bita je unaprijed podešeno na vrijednost 10010, dok su vrijednosti dva najniža bita A1 i A0 određene položajem SMD džampera, te u ovom slučaju imaju vrijednosti 00. Na osnovu toga, određena je adresa slave uređaja i ona iznosi **0x48**. Pored adresnog bajta, ADS7828 posjeduje i komandni bajt pomoću kog se ovaj uređaj konfiguriše. Za potrebe našeg projekta potrebno je omogućiti single-ended ulaz, na kanalu CH0, te uključiti A/D konvertor i njegovu internu referencu. Ovakvim izborom konfiguracije vrijednost komandnog bajta iznosi **0x8F**. Interna referenca, čija vrijednost napona iznosi 2.5 V, omogućava opseg napona na ulazu od 0 do 3.3 V, što predstavlja napon napajanja ovog uređaja. 

Kombinovanom I2C transakcijom se vrši konfiguracija ovog uređaja, tj. slanjem vrijednosti komandnog bajta, te čitanje dobijenih digitalnih vrijednosti. Dobijeni 12-bitni podaci, pakuju se u 2 bajta, s tim da prvo stiže MSB bajt, a nakon njega i LSB bajt. Detaljnija implementacija ove kombinovane I2C transakcije je dostupna u fajlu ADC/adc.c.

### Testiranje ADC 12 Click modula
Prije testiranja razvijenog programa potrebno je povezati ADC 12 Click modul sa odgovarajućim pinovima Rasberry Pi platforme. Na osnovu rasporeda pinova Raspberry Pi platforme dostupnog [ovde](https://pinout.xyz/), te [dokumentacije](https://download.mikroe.com/documents/datasheets/ADS7828_datasheet.pdf) ADC 12 Click modula, način povezivanja prikazan je u tabeli ispod:

| Raspberry Pi Pin | ADC 12 Click pin |
| :------: | :------: | 
| Power 3.3 V | 3.3 V |
| GPIO 2 (I2C1 SDA) | SDA |
| GPIO 3 (I2C1 SCL) | SCL |
| Ground | GND |

Prvi vid testiranja ovog A/D konvertora je sproveden dovođenjem napona sa potenciometra na njegov ulaz (CH0). Rezultati testiranja prikazani su na slikama ispod:

