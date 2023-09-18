#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Dados de Conexão com o Wifi (Usuário e Senha da Rede)
const char* ssid = "Defina Sua Rede";
const char* password = "Defina sua Senha";

// GPIO de conexão com o sensor de temperatura DS18B20 
const int oneWireBus = A4;

// Inicialize a instância OneWire para se comunicar com dispositivos OneWire
OneWire oneWire(oneWireBus);

// Passe nossa referência oneWire para o sensor Dallas Temperature
DallasTemperature sensors(&oneWire);

// Definindo a porta de comunicação
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  sensors.begin();
  delay(10);

  // Printando dados de conexão
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  // Iniciando conexão com Wifi
  WiFi.begin(ssid, password);

  // Loop de repetição durante a conexão com o wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Aguarde");
    Serial.print(".");
  }

  // Printando o endereço IP do servidor web
  Serial.println("");
  Serial.println("Conexão WiFi Realizada");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());

  // Inicie o servidor web
  server.begin();
}

void loop() {
  // Aguarde uma conexão de cliente
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Espere até que o cliente envie algum dado
  Serial.println("Novo cliente conectado");
  while (!client.available()) {
    delay(1);
  }

  // Leia a primeira linha da solicitação HTTP
  String request = client.readStringUntil('\r');
  client.flush();

  // Verifique se a solicitação é para a raiz ("/")
  if (request.indexOf("/ ") != -1) {
    // Prepare a resposta HTTP com os dados do sensor de temperatura
    String temperatureC = getTemperature(); // Obtenha a temperatura aqui

    //Página HTML com o resultado das leituras
    String html = "<html><body style='font-family: Arial, sans-serif;'>";
    html += "<h1 style='color: #333;'>Sistema de Monitoramento de Temperatura</h1>";
    html += "<h3 style='color: #555;'>Leitura do Sensor de Temperatura</h3>";
    html += "<div style='border: 2px solid #ccc; padding: 20px;'>";
    html += "<p style='font-size: 24px; color: #007BFF;'>Temperatura: " + temperatureC + " °C</p>";
    html += "</div>";
    html += "</body></html>";

    // Envie a resposta HTTP de volta ao cliente
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(html);
    client.println();

    // Encerre a conexão
    delay(1);
    Serial.println("Cliente desconectado");
    Serial.println("");
  }
}

// Função de leitura do Sensor de Temperatura
String getTemperature() {
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  return String(temperatureC);
}
