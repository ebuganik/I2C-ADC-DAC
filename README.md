# I2C-ADC-DAC
Projektni zadatak se zasniva na radu sa pločicama ADC 12 Click i DAC 10 Click proizvođača Mikroelektronika. Date pločice je potrebno konfigurisati pomoću I2C protokola, tako da se omogući prikazivanje signala dovedenog na ulaz analogno/digitalnog konvertora na standardnom izlazu, te generisanje osnovnih tipova analognog signala: trougaonog, pravougaonog i sinusnog na izlazu digitalno/analognog konvertora.
## Konfiguracija Raspberry Pi platforme
Prije početka rada na projektnom zadatku potrebno je sprovesti niz koraka koji se odnose na konfiguraciju korištene Raspberry Pi platforme, tj. formatiranje SD kartice i preuzimanje i instalacije odgovarajućeg operativnog sistema. Nakon postavljanja korisničkog imena i hostname-a prilikom preuzimanja Raspberry Pi Imager-a, sljedeći korak jeste povezivanje sa Raspberry Pi platformom putem sljedeće komande:

``` ssh username@hostname.local ```

Nakon toga, potrebno je postaviti statičku IP adresu editovanjem fajla **/etc/dhcpcd.conf**. Detaljniji proces je objašnjen u dokumentaciji dostupnoj na kursu predmeta.

## Rad sa ADC 12 Click modulom
### Konfiguracija ADS7828 uređaja
ADC 12 Click je razvojna pločica na kojoj se nalazi ADS7828, 12-bitni uređaj koji posjeduje I2C interfejs i 8 kanala za analogne ulaze. Kako se on u kombinaciji sa Raspberry Pi platformom ponaša kao slave uređaj, prvo je potrebno odrediti njegovu I2C adresu. U ovom slučaju adresa ovog slave uređaja je sedmobitna, a najviših 5 bita je unaprijed podešeno na vrijednost 10010, dok su vrijednosti dva najniža bita A1 i A0 određene položajem SMD jumper-a, te u ovom slučaju imaju vrijednosti 00. Na osnovu toga, određena je adresa slave uređaja i ona iznosi **0x48**. Pored adresnog bajta, ADS7828 posjeduje i komandni bajt pomoću kog se ovaj uređaj konfiguriše. Za potrebe našeg projekta potrebno je omogućiti single-ended ulaz, na kanalu CH0, te uključiti A/D konvertor i njegovu internu referencu. Ovakvim izborom konfiguracije vrijednost komandnog bajta iznosi **0x8F**. Interna referenca, čija vrijednost napona iznosi 2.5 V, omogućava opseg napona na ulazu od 0 do 3.3 V, odnosno od 0 do napona napajanja uređaja. 

Kombinovanom I2C transakcijom se vrši konfiguracija ovog uređaja, tj. slanjem vrijednosti komandnog bajta, te čitanje dobijenih digitalnih vrijednosti. Dobijeni 12-bitni podaci pakuju se u 2 bajta, s tim da prvo stiže MS bajt, a nakon njega i LS bajt. Na slici ispod prikazana je jedna kombinovana I2C transakcija, na kojoj se redom mogu uočiti sljedeći parametri: start sekvenca, sedmobitna adresa slave uređaja 1001000 (0x48), write pristup (0), te ACK od strane slave uređaja (0). Nakon toga se šalje komandni bajt 10001111 (0x8F) i dobijamo ACK (0). Zatim ide Repeated Start sekvenca, adresa slave uređaja 0x48, te Read pristup (1). Nakon toga slave uređaj ponovo šalje ACK (0), te dva bajta podataka. Detaljnija implementacija ove kombinovane I2C transakcije je dostupna u fajlu ADC/adc.c. Takođe, sa slike se vidi i brzina I2C transakcije i ona iznosi 50 kHz.

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


Prvi vid testiranja ovog A/D konvertora je sproveden dovođenjem napona sa potenciometra na njegov ulaz (CH0). Rezultati testiranja prikazani su na slikama ispod, prilikom čega je upoređena vrijednost napona izmjerenog multimetrom sa vrijednostima dobijenih od A/D konvertora. Interpretacija sirovih podataka je izvršena korištenjem sljedeće formule:

**$V_{out} = \frac{ADC{data} * V_{ref}}{2^N}$**

gdje $V_{ref}$ predstavlja referentni napon od 2.51 V, a $2^N$ kod 12-bitnog uređaja predstavlja vrijednost od 4096.

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
### Konfiguracija DAC53401 uređaja

DAC 10 Click za svoju osnovu koristi DAC53401, 10-bitni digitalno-analogni konvertor Texas Instruments. S obzirom na to da posjeduje mogućnost rada sa I2C interfejsom, ponajprije je potrebno odrediti adresu ovog slave uređaja kako bi se mogao ispravno konfigurisati. DAC53401 raspolaže sa general i broadcast adresom, u slučaju da se koristi više DAC53401 u komunikaciji. U ovom slučaju potrebna nam je samo general adresa, koja je kao i kod A/D konvertora sedmobitna i određena je preset bitima (fabrički, unaprijed definisanim) 1001 koji predstavljaju više bite te sa tri niža koja zavise od položaja pina A0. Kako je položaj pina A0 na DAC10 Click prema AGND, preostala tri bita su određena sa 000, pa tako adresni bajt iznosi **0x48**.

Konfigurisanje DAC podrazumijeva upisivanje odgovarajućih vrijednosti u nekoliko NVM ili non-volatile registara ovog konvertora. Na primjer, moguće je obezbijediti rad sa internom referencom koja iznosi 1.21 V ili pak eksternom referencom koja zavisi od dovedenog napona napajanja (3 V ili 5.5 V). U radu koji je prikazan u nastavku koristi se napon napajanja od 3.3 V, sa omogućenom eksternom referencom, koja prema tome iznosi 3.3 V i ona se može podesiti upravo korištenjem GENERAL CONFIG registra. U nekim slučajevima potrebno je podesiti i opsege signala koji se prikazuje na izlazu i to upisom u DAC_MARGIN_LOW i _HIGH registre, a ako ipak treba da upisujemo vrijednosti odmjeraka koji će se konvertovani u analogne vrijednosti prikazati na izlazu tada će biti neophodno upisati vrijednosti istih u DAC_DATA registar. Naravno, da bismo omogućili da se sadržaj upiše u pomenute NVM registre, neophodno je setovati NVM_PROG bit u TRIGGER registru. Više detalja o tome kako je u kojem zadatku vršeno konfigurisanje pojašnjeno je u korištenim fajlovima koji se nalaze u folderu ```DAC```.

Potrebno je pomenuti i kako je moguće pročitati i device ID, koji se nalazi u STATUS registru. S obzirom na to da je u pitanju DAC53401-Q1, kako je navedeno i u [datasheet](https://download.mikroe.com/documents/datasheets/DAC53401_datasheet.pdf)-u ovog D/A konvertora, ID iznosi **0x0C**. Minimalna povezivanja DAC sa master uređajem koji je u ovom slučaju Raspberry Pi mogu se ostvariti kao i u prethodno opisanom radu sa ADC 12 Click modulom, u tabeli u poglavlju ```Testiranje ADC 12 Click modula```.
Pored čitanja Device ID-a, tu je i dodatna transakcija za čitanje vrijednosti koju smo upisali u GENERAL_CONFIG registar kako bi se potvrdila ispravna konfiguracija.
<p align="center"> 
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116347913/6357462a-e78b-40d1-8a99-403f864e6e27" width = "500", height = "70">

## Generisanje talasnih oblika na izlazu DAC konvertora

Za prikaz nekih od talasnih oblika na izlazu DAC moguće je iskoristiti Continuous Waveform Generation Mode (CWG) koji predstavlja jednu od naprednih funkcionalnosti DAC53401. Tako je moguće odgovarajućim podešavanjem GENERAL_CONFIG registra na izlazu izgenerisati trougaoni, testerasti ili pak pravougaoni oblik signala, sa programabilnim vrijednostima bita koji određuju CODE_STEP (odnosno vrijednost za koju se izlaz DAC mijenja sa promjenom na ulazu) te SLEW_RATE (brzina promjene izlaznog signala), a sa opsegom koji se podešava u prethodno pomenutim registrima za podešavanje margina. Isto tako, podešavanjem pomenutih parametara moguće je odrediti i frekvenciju signala koji će se izgenerisati. 

Pri generisanju traženih talasnih oblika bilo je neophodno uzeti u obzir opseg napona koji će se pojaviti na izlazu. Dokumentacija DAC 10 Click nalaže da je izlazni opseg napona od 0 V do 5.5 V. S obzirom na to da koristimo napon napajanja od 3.3 V te da smo njega izabrali da nam bude eksterna referenca, na izlazu DAC možemo očekivati signal do 3.3 V.

Talasni oblici prikazani u nastavku izgenerisani su python skriptama koje se nalaze u folderu **Python**, a za prikaz podataka .csv fajlova izgenerisanih na osciloskopu.

### Generisanje trougaonog signala
Frekvencija trougaonog signala određuje se sljedećim izrazom:

<p align="left">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116280871/c6d754b5-37d1-4681-9113-bd5a9f07607a" width = "500",  height = "80"> 

U slučaju generisanja trougaonog signala razmatrana su dva slučaja:

Prva, gdje je vrijednost gornje granice - margine 1.65 V (na polovini opsega napona na izlazu). Slew rate iznosi 25.6 us x 1.50, a code step 1 LSB (odnosno Vref/1024 = 3.22 mV). Proračunata frekvencija iznosi ...

 <p align="center">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116280871/62cebfc7-e036-4951-ac7f-7398d1009b0a" width = "625", height = "350">                    

Druga, gdje je vrijednost gornje granice - margine 3.3 V. Slew rate iznosi 25.6 us, a code step 1 LSB. Proračunata frekvencija iznosi ...
 <p align="center">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116280871/6f8e708f-bfb7-4903-83dd-b31e2abb4893" width = "625", height = "350">  

Implementacija korištene I2C transakcije nalazi se u folderu **DAC**, u fajlovima **triangle_lower_amp.c** i **triangle_higher_amp.c**. 

### Generisanje pravougaonog signala
Frekvencija pravougaonog signala određuje se sljedećim izrazom:

<p align="left">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116280871/1cafb3a5-8205-46f9-9779-656d91064528" width = "250",  height = "60"> 
 
U slučaju generisanja pravougaonog signala razmatrana su dva slučaja:
Prva, gdje je vrijednost gornje granice - margine 1.65 V (na polovini opsega napona na izlazu). Slew rate iznosi 4 us, a code step 1 LSB. Proračunata frekvencija iznosi 125 kHz.

 <p align="center">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116280871/c819135b-87f4-4fec-8b4f-84a5b4d2a180" width = "625", height = "350">   

Druga, gdje je vrijednost gornje granice - margine 3.3 V. Slew rate iznosi 25.6 us, a code step 1 LSB. Proračunata frekvencija iznosi približno 20 kHz.

 <p align="center">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116280871/b3e9fca8-285f-4218-9197-6ea407279da1" width = "625", height = "350">  

Daju se primijetiti značajne razlike u prikazu pravougaonog signala na prvoj i drugoj slici, tačnije, uticaj promjenjivog slew rate-a koji je u prvom slučaju rezultovao sporijom promjenom rastuće i opadajuće ivice signala tako da je on poprimio više trapezoidni oblik, nego pravougaoni.

Implementacija korištene I2C transakcije nalazi se u folderu **DAC**, u fajlovima **rectangle_lower_amp.c** i **rectangle_higher_amp.c**.

### Generisanje sinusnog signala

Za razliku od korištenja CWG-a u generisanju pomenutih talasnih oblika na izlazu DAC, kako bismo izgenerisali sinusni talasni oblik bilo je neophodno primijeniti neki drugi pristup. 

Potrebno je smjestiti odmjerke sinusnog signala u DAC_DATA registar i na izlazu omogućiti signal koji će da bude periodičan. Pritom je potrebno uzeti u obzir kako od dvobajtnog sadržaja NVM registra DAC_DATA svega 10 bita prikazuje decimalnu vrijednost svakog upisanog odmjerka, dok se preostali biti tretiraju kao *don't care*. Od značaja bi bilo formirati lookup tabelu iz koje će se odmjerci čitati i uz određeno kašnjenje prikazivati na izlazu DAC.

Iz pomenutih razloga, za generisanje odmjeraka sinusnog signala korištena je python skripta ``` sineGenerator.py ```, u kojoj su uključeni faktori poput frekvencije samog signala, ali i frekvencije odmjeravanja, broj tačaka (samples) u kojem želimo naš sinusni signal, kao i njegova amplituda i DC ofset, kako signal treba da se nađe u opsegu od 0 do 3.3 V. Pokretanjem skripte u terminalu se ispisuju 10-bitne decimalne vrijednosti odmjeraka, na koje se potom dodaju *don't care* biti i tako formiraju dvobajtni podaci, koji se zatim upisuju u sine_wave_samples.txt fajl, tačnije lookup tabelu koja se potom koristi u kodu za transakciju.

Primjer generisanja sinusnog signala od približno 50 Hz, sa frekvencijom odmjeravanja 1kHz i brojem odmjeraka 120 prikazan je na sljedećoj slici:

 <p align="center">
<img src = "https://github.com/ebuganik/I2C-ADC-DAC/assets/116280871/baf9dfea-72e5-4315-bfd0-4602b8916a08" width = "600", height = "400">   

Python skripta za generisanje odmjeraka sinusnog signala nalazi se u fajlu **Python/sineGenerator.py**, dok je implementacija korištene I2C transakcije smještena u fajlu **DAC/sine.c**.

## Replika ulaznog signala A/D konvertora na D/A konvertoru
U slučaju da se adrese ova dva slave uređaja razlikuju, što je moguće postići postavljanjem SMD jumper-a zaduženih za konfigurisanje njihovih I2C adresa u drugačije položaje od trenutnih, postoji mogućnost da se na izlazu D/A konvertora dobije replika signala koji je na ulazu A/D konvertora.
U tom smislu potrebno je inicirati kombinovanu I2C transkciju koja će prvo konfigurisati A/D konvertor i čitati podatke koje šalje, zatim transakciju koja će nam potvrditi da komuniciramo sa odgovarajućim uređajem čitanjem Device ID-a iz statusnog registra D/A konvertora. Pored toga potrebno je konfigurisati D/A konvertor upisivanjem odgovarajućih podataka u GENERAL_CONFIG i TRIGGER registre kao u prethodnim poglavljima, te dodatna transakcija koja će nam omogućiti upisivanje podataka dobijenih od A/D konvertora u DAC_DATA registar D/A konvertora.
Podaci koje ćemo dobiti od A/D konvertora potrebno je prilagoditi arhitekturi D/A konvertora, jer se radi o uređajima sa različitim rezolucijama. Pored toga, ovi uređaji imaju i različite referentne napone te je zbog toga neophodno izvršiti manipulaciju sirovih podataka sa A/D konvertora.
Sirove, 12-bitne podatke A/D konvertora ćemo prilagoditi D/A konvertoru tako što ih prvo podijelimo sa 4 (4096/1024 = 4), zatim ćemo ih pomnožiti sa referentnim naponom A/D konvertora (2.5 V) i podijeliti sa referentnim naponom D/A konvertora (3.3 V). Na ovaj način smo pronašli digitalni ekvivalent datog odmjerka signala. Na kraju, tako dobijene podatke potrebno je pomjeriti za dva bita ulijevo, kako bismo dobili odgovarajuće 12-bitno poravnanje koje D/A konvertor zahtijeva. Detaljnija implementacija ovog postupka dostupna je u fajlu **adc-dac.c**.





