#include <ESP8266WiFi.h>

// Información de nuestro WIFI
const char *ssid = "RED_3";
const char *password = "ABCD56789";

// Datos para una IP estática
IPAddress ip(192, 168, 1, 24);     // IP de servidor
IPAddress gateway(192, 168, 1, 1); // Puerta de enlace
IPAddress subnet(255, 255, 255, 0); 

// Definimos el pin donde está conectado el led
int ledPin = 2; 

// Puerto 80 TCP, este puerto se usa para la navegación web http
WiFiServer server(80);
 
void setup() {
  Serial.begin(115200); // Iniciamos comunicación serial
  delay(10);
  
  pinMode(ledPin, OUTPUT);  // Pin del Led como salida
  digitalWrite(ledPin, LOW);  // Ponemos la salida de led en bajo
   
  // Conectándose al Wifi
  Serial.println();
  Serial.println();
  Serial.print("Conectándose a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); // Wifi en modo estación
  WiFi.config(ip, gateway, subnet); // Configuramos la IP estática
  WiFi.begin(ssid, password); // Iniciamos conexión con el nombre y la contraseña del wifi que indicamos

  // Mientras se conecta se imprimirán puntos
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Cuando se conecte lo imprimimos
  Serial.println("");
  Serial.println("WiFi Conectado");
   
  // Iniciamos el esp como servidor web
  server.begin();
  Serial.println("Servidor iniciado");
 
  // Imprimimos la dirección IP
  Serial.print("Usa esta URL para comunicar al ESP: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  // El servidor siempre estará esperando a que se conecte un cliente
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("Nuevo cliente"); // Cuando un cliente se conecte vamos a imprimir que se conectó
  while (!client.available()) {  // Esperamos a que el cliente mande una solicitud
    delay(1);
  }

  // Leemos la primera línea de la solicitud y la guardamos en la variable string request
  String request = client.readStringUntil('\r');
  Serial.println(request); // Imprimimos la solicitud
  client.flush(); // Descartamos los datos que se han escrito en el cliente y no se han leído

  // Relacionamos la solicitud
  if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(ledPin, LOW);
  }
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH);
  } 
  
  if (request.indexOf("/LED=BLINK") != -1) {
    for (int i = 0; i < 10; i++) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
    }
  }

  // Respuesta del servidor web
  if (request.indexOf("/enviarTexto") != -1) {
      int startIndex = request.indexOf("texto=") + 6;
        int endIndex = request.indexOf(" HTTP");
      String texto = request.substring(startIndex, endIndex);
      Serial.println("Texto recibido: " + texto);

      // Imprimir solo la palabra
      Serial.println("Palabra recibida: " + texto);
  }
  
  client.println("HTTP/1.1 200 OK"); // La respuesta empieza con una línea de estado  
  client.println("Content-Type: text/html"); // Empieza el cuerpo de la respuesta indicando que el contenido será un documento HTML
  client.println(""); // Ponemos un espacio
  client.println("<!DOCTYPE HTML>"); // Indicamos el inicio del Documento HTML
  client.println("<html lang=\"en\">");
  client.println("<head>");
  client.println("<meta charset=\"UTF-8\">");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"); // Para que se adapte en móviles
  client.println("<title>ARC-MATRIZ LED</title>");
  client.println("</head>");
  client.println("<body>");
  client.println("<br><br>");
  
  client.println("<h1 style=\"text-align: center;\">Servidor Web ESP8266</h1>");
  
  client.println("<p style=\"text-align: center;\">");
  client.println("Click <a href=\"/LED=ON\">Aqui</a> para encender LED en el pin 2 ON<br>"); // Oración que contiene un hipervínculo
  client.println("Click <a href=\"/LED=OFF\">Aqui</a> para apagar el led en el pin 2 OFF<br>");
  client.println("Click <a href=\"/LED=BLINK\">Aqui</a> para parpadear el led en el pin 2<br> <br>");
  client.println("<input type='text' id='textoInput' placeholder='Escribe tu texto aquí' class='input-caja-texto'>");
  client.println("<br><br>");
  client.println("<button class='botonUno' id='boton-enviar' onclick='enviarTexto()'>Enviar texto</button>");
  client.println("<button onclick=location.href=\"/LED=ON\"> Conectar LED</button> <br> <br>"); // Botón sencillo que contiene hipervínculo
  client.println("<button onclick=location.href=\"/LED=OFF\" >Desconectar LED </button> <br> <br>");
  client.println("<button onclick=location.href=\"/LED=BLINK\">Parpadear LED </button> <br> <br>");
  client.println("<script>");
  client.println("function enviarTexto() {");
  client.println("var texto = document.getElementById('textoInput').value;");
  client.println("fetch('/enviarTexto?texto=' + encodeURIComponent(texto))");
  client.println(".then(response => {");
  client.println("if (!response.ok) {");
  client.println("throw new Error('Error al enviar el texto');}");
  client.println("return response.text(); })");
  client.println(".then(data => {");
  client.println("console.log('Texto enviado con éxito:', data);})");
  client.println(".catch(error => {");
  client.println("console.error('Error:', error)});}");
  client.println("</script>");
  client.println("</p>");
  client.println("</body>");
  client.println("</html>"); // Terminamos el HTML
 
  delay(1);
  Serial.println("Cliente desconectado"); // Imprimimos que terminó el proceso con el cliente desconectado
  Serial.println("");
}
