#include <WiFi.h>           // Biblioteca para wifi)
#include <HTTPClient.h>     // Biblioteca para requisições HTTP
#include <DHT.h>            // Biblioteca para o sensor DHT11

// Configuração dos sensores
#define DHTPIN 18         // Pino digital GP18 para o DHT11
#define DHTTYPE DHT11     // Tipo do sensor DHT11
#define MQ135PIN 28       // Pino analógico GP28 para o MQ135

DHT dht(DHTPIN, DHTTYPE);

// Configuração dos LEDs para status
#define LED_WIFI_PIN 11    // LED aceso se conectado ao Wi-Fi
#define LED_SUCCESS_PIN 12 // LED aceso se os dados forem enviados com sucesso
#define LED_FAIL_PIN 13    // LED aceso se houver falha no envio dos dados

// Configuração do Wi-Fi
const char* ssid = "COPELLI-2G";       // Nome da rede wifi
const char* password = "copelli1";     // Senha da rede wifi

// URL do servidor para onde os dados serão enviados
const char* serverUrl = "http://192.168.0.38:3000/api/submitData";

WiFiClient client;  // Instância para conexão HTTP

void setup() {
  Serial.begin(115200);
  // Configura os pinos dos LEDs como saída
  pinMode(LED_WIFI_PIN, OUTPUT);
  pinMode(LED_SUCCESS_PIN, OUTPUT);
  pinMode(LED_FAIL_PIN, OUTPUT);

  // Conectar ao wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  // Acende o LED de Wi-Fi (pino 11)
  digitalWrite(LED_WIFI_PIN, HIGH);

  // Inicializa o sensor DHT
  dht.begin();
}

// Função para ler sensor MQ-135
float readMQ135() {
  int sensorValue = analogRead(MQ135PIN);  // Leitura do pino analógico GP28
  return sensorValue;
}

// Função para calcular as concentrações dos gases para teste
void getGasConcentration(int mq135Value, float &alcool, float &benzeno, float &co2, float &fumaca) {
  alcool = mq135Value * 0.1;   // Calculo apenas para teste
  benzeno = mq135Value * 0.2;  
  co2 = mq135Value * 0.05;     
  fumaca = mq135Value * 0.15;  
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    // Leitura dos sensores:
    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();
    int qualidadeAr = readMQ135();

    // Verifica se a leitura do DHT11 foi bem-sucedida
    if (isnan(temperatura) || isnan(umidade)) {
      return;
    }

    // Variáveis para os gases
    float alcool, benzeno, co2, fumaca;
    getGasConcentration(qualidadeAr, alcool, benzeno, co2, fumaca);

    // Preparando os dados para envio (incluindo o ID da placa)
    String nodeID = WiFi.macAddress();
    String payload = "{\"id\": \"" + nodeID + 
                     "\", \"temperatura\": " + String(temperatura) + 
                     ", \"umidade\": " + String(umidade) + 
                     ", \"qualidadeAr\": " + String(qualidadeAr) +
                     ", \"alcool\": " + String(alcool) +
                     ", \"benzeno\": " + String(benzeno) +
                     ", \"CO2\": " + String(co2) +
                     ", \"fumaca\": " + String(fumaca) + "}";

    // Envia os dados para o servidor via HTTP POST
    HTTPClient http;
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      // Dados enviados com sucesso: acende LED no pino 12 e apaga LED de falha
      digitalWrite(LED_SUCCESS_PIN, HIGH);
      digitalWrite(LED_FAIL_PIN, LOW);
    } else {
      // Falha ao enviar os dados: acende LED no pino 13 e apaga LED de sucesso
      digitalWrite(LED_SUCCESS_PIN, LOW);
      digitalWrite(LED_FAIL_PIN, HIGH);
    }
    http.end();
  } else {
    WiFi.begin(ssid, password);
    // Desliga os LEDs de status se não conectado
    digitalWrite(LED_WIFI_PIN, LOW);
    digitalWrite(LED_SUCCESS_PIN, LOW);
    digitalWrite(LED_FAIL_PIN, LOW);
  }

  delay(60000);  // Aguarda 1 minuto antes de enviar novamente
}


