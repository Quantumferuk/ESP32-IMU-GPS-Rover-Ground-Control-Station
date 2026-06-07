#include <ESP8266WiFi.h>
#include <espnow.h>

#define ESPNOW_CHANNEL 1

// ESP32 rover STA MAC adresin.  //buraya hedef mac adresini girin.
uint8_t roverMac[] = {};

// Python'dan gelip ESP32'ye giden hareket komut paketi
typedef struct {
  char komut;
} VeriPaketi;

VeriPaketi gidenVeri;

// ESP32'den NodeMCU'ya gelen Telemetri paketi (BİREBİR AYNI OLMALI!)
typedef struct __attribute__((packed)) {
  uint8_t gps_valid;
  int32_t lat_e7;
  int32_t lng_e7;
  uint8_t satellites;
  uint16_t hdop_x100;
  int16_t altitude_m;
  uint16_t speed_kmph_x100;

  int16_t ax_mg;
  int16_t ay_mg;
  int16_t az_mg;

  int16_t gx_dps_x100;
  int16_t gy_dps_x100;
  int16_t gz_dps_x100;

  int16_t yaw_deg_x100;

  uint32_t packet_counter;
} TelemetryPacket;

TelemetryPacket gelenTelemetry;

void veriGonderildi(uint8_t *mac_addr, uint8_t sendStatus) {}

void veriGeldi(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len) {
  if (len != sizeof(TelemetryPacket)) {
     Serial.print("HATA: Paket Boyutu Uyusmuyor! Nodemcu= ");
     Serial.print(sizeof(TelemetryPacket));
     Serial.print(" | ESP32= ");
     Serial.println(len);
     return;
  }

  // Paket doğruysa çöz
  memcpy(&gelenTelemetry, incomingData, sizeof(gelenTelemetry));

  // --- 1. GPS + HIZ Verisi ---
  if (gelenTelemetry.gps_valid == 1) {
    double enlem = gelenTelemetry.lat_e7 / 10000000.0;
    double boylam = gelenTelemetry.lng_e7 / 10000000.0;
    float hiz = gelenTelemetry.speed_kmph_x100 / 100.0;

    Serial.print("GPS:");
    Serial.print(enlem, 7);
    Serial.print(",");
    Serial.print(boylam, 7);
    Serial.print(",");
    Serial.println(hiz, 1);
  }
  
  // --- 2. IMU (Yaw + İvmeölçer X,Y,Z) Verisi ---
  float yaw = gelenTelemetry.yaw_deg_x100 / 100.0;
  
  // ESP32'den ivme değerleri milig-kuvveti (mg) cinsinden integer geliyor.
  float ax = gelenTelemetry.ax_mg / 1000.0;
  float ay = gelenTelemetry.ay_mg / 1000.0;
  float az = gelenTelemetry.az_mg / 1000.0;

  // IMU string'ini Python'un parçalayabileceği şekilde yolla
  // Format: IMU:yaw,ax,ay,az
  Serial.print("IMU:");
  Serial.print(yaw, 2);
  Serial.print(",");
  Serial.print(ax, 3);
  Serial.print(",");
  Serial.print(ay, 3);
  Serial.print(",");
  Serial.println(az, 3);
}

void komutGonder(char komut) {
  gidenVeri.komut = komut;
  esp_now_send(roverMac, (uint8_t *)&gidenVeri, sizeof(gidenVeri));
}

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  wifi_set_channel(ESPNOW_CHANNEL);

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW baslatilamadi!");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(veriGonderildi);
  esp_now_register_recv_cb(veriGeldi); 

  if (esp_now_add_peer(roverMac, ESP_NOW_ROLE_COMBO, ESPNOW_CHANNEL, NULL, 0) != 0) {
    Serial.println("ESP32 rover peer olarak eklenemedi!");
    return;
  }
  
  Serial.println("NodeMCU (Motor+GPS+ IMU_XYZ) Hazir.");
}

void loop() {
  if (Serial.available()) {
    char komut = Serial.read();
    if (komut == 'F' || komut == 'f') komutGonder('F');
    else if (komut == 'B' || komut == 'b') komutGonder('B');
    else if (komut == 'L' || komut == 'l') komutGonder('L');
    else if (komut == 'R' || komut == 'r') komutGonder('R');
    else if (komut == 'S' || komut == 's') komutGonder('S');
  }
}