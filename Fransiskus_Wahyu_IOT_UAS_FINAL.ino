/*Simple IOT Project
 * Ujian Akhir Semester (UAS)
 * Created by : Fransiskus Wahyu S 
 * NIM : 515-0711-039 / TE / UTY
 * Menggunakan library EtherCard.h untuk ethernet modul dengan chip ENC28J60
 * Yogyakarta, Mei 2018
 */

#include <EtherCard.h> 
#define REQUEST_RATE 5000

#define STATIC 1  //1 untuk aktifkan seting IP statis, jika 0 DHCP client aktif
#if STATIC
// alamat IP (jika diatur statis)
static byte myip[] = { 192,168,0,254 };
static byte gwip[] = { 192,168,0,1 };
#endif

char* on  = "ON";
char* off = "OFF";
boolean L1Stat, L2Stat, L3Stat, L4Stat;
char* L1Label; char* L2Label; char* L3Label; char* L4Label;
char* L1linkLabel; char* L2linkLabel; char* L3linkLabel; char* L4linkLabel;
int L1=3,L2=4,L3=5,L4=6; //mendeklarasikan pin Lampu 1 hingga 4
//alamat fisik ethernetcard (MAC Address)
static byte mac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
byte Ethernet::buffer[800]; // tcp/ip send and receive buffer


void setup(){
  Serial.begin(9600);
  Serial.println("\n[HelloWorld!]");
  setIP(); //memanggil fungsi setIP
  int a;
  for (a=3;a<=6;a++){
    pinMode (a,OUTPUT);
    digitalWrite(a, LOW);  
    }
  //menentukan kondisi awal
  L1Stat = false;  L1Label = off; L1linkLabel = on;
  L2Stat = false;  L2Label = off; L2linkLabel = on;
  L3Stat = false;  L3Label = off; L3linkLabel = on;
  L4Stat = false;  L4Label = off; L4linkLabel = on;
}
void loop(){
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
 
  if(pos) {
       //mendefinikan kondisi sesuai dengan parameter acuan yaitu laman web
    if(strstr((char *)Ethernet::buffer + pos, "GET /?ON1") != 0) {
      L1Stat = true;
      L1Label = on;
      L1linkLabel = off;
     } else if (strstr((char *)Ethernet::buffer + pos, "GET /?OFF1") != 0) {
      L1Stat = false;
      L1Label = off;
      L1linkLabel = on;
     } else if (strstr((char *)Ethernet::buffer + pos, "GET /?ON2") != 0) {
      L2Stat = true;
      L2Label = on;
      L2linkLabel = off;
     } else if (strstr((char *)Ethernet::buffer + pos, "GET /?OFF2") != 0) {
      L2Stat = false;
      L2Label = off;
      L2linkLabel = on;
     } else if (strstr((char *)Ethernet::buffer + pos, "GET /?ON3") != 0) {
      L3Stat = true;
      L3Label = on;
      L3linkLabel = off;
     } else if (strstr((char *)Ethernet::buffer + pos, "GET /?OFF3") != 0) {
      L3Stat = false;
      L3Label = off;
      L3linkLabel = on;
     } else if (strstr((char *)Ethernet::buffer + pos, "GET /?ON4") != 0) {
      L4Stat = true;
      L4Label = on;
      L4linkLabel = off;
     } else if (strstr((char *)Ethernet::buffer + pos, "GET /?OFF4") != 0) {
      L4Stat = false;
      L4Label = off;
      L4linkLabel = on;
    }
    
//melakukan output sesuai dengan kondisi (status) pada variabel L1 hingga L4
    digitalWrite(L1, L1Stat); 
    digitalWrite(L2, L2Stat);
    digitalWrite(L3, L3Stat);
    digitalWrite(L4, L4Stat);
//menampilkan antarmuka ke pengguna menggunakan fitur Buffer Filler
    BufferFiller bfill = ether.tcpOffset();
    bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\n"
      "Content-Type: text/html\r\nPragma: no-cache\r\n\r\n"
      "<html><head><meta name='viewport' content='width=200px'/>"
      "<body style='background-color:#717272;'>"
      "<title>Fransiskus IOT Project</title>"
      //memuat file css yang ada di server (github saya)
      "<link rel='stylesheet' type='text/css' href='https://rawgit.com/franzwahyu/test/master/tombol.css'>"
       "</head>"
      "<h3 style='text-align:center'>Kontrol Lampu Online</h3>"
      "<div style='position:absolute;width:200px;height:450px;top:30%;left:50%;margin:-100px 0 0 -100px'>"
      "<br><div style='text-align:center'>"
      //menampilkan user interface tombol
      "<div style='font:bold 14px verdana;text-align:center'>Lampu 1 $S</div>"
      "<a href='/?$S1'><button class='button'>Turn $S!</button></a><br>"
      "<br><div style='font:bold 14px verdana;text-align:center'>Lampu 2 $S</div>"
      "<a href='/?$S2'><button class='button'>Turn $S!</button></a><br>"
      "<br><div style='font:bold 14px verdana;text-align:center'>Lampu 3 $S</div>"
      "<a href='/?$S3'><button class='button'>Turn $S!</button></a><br>"
      "<br><div style='font:bold 14px verdana;text-align:center'>Lampu 4 $S</div>"
      "<a href='/?$S4'><button class='button'>Turn $S!</button></a>"
      "<br><br>"
      "<div style='font:bold 12px arial'>Kontrol Lampu v1.0<br>By:Fransiskus Wahyu S<div>"
      "</div></div></body></html>"
      "<meta http-equiv='refresh' content='3;url=/'>"
      ),
//mendefinisikan isi format buffer filler (sesuai urutan $S dipanggil)
      L1Label, L1linkLabel, L1linkLabel, 
      L2Label, L2linkLabel, L2linkLabel,
      L3Label, L3linkLabel, L3linkLabel,
      L4Label, L4linkLabel, L4linkLabel
      );   
     ether.httpServerReply(bfill.position());
    }
}

void setIP(){
 ether.begin(sizeof Ethernet::buffer, mac);
  #if STATIC
    ether.staticSetup(myip, gwip);
  #else
    Serial.println("Searching DHCP");
    if (!ether.dhcpSetup()) Serial.println("DHCP failed");
    if (ether.dhcpSetup()) Serial.println("DHCP Success");
  #endif
    ether.printIp("IP   : ", ether.myip);
    ether.printIp("GW   : ", ether.gwip); 
    ether.printIp("MASK : ", ether.netmask);   
    ether.printIp("DNS  : ", ether.dnsip); 
}

