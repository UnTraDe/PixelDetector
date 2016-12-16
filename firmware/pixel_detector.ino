#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define _DEBUG

String _request = String("POST /fcm/send HTTP/1.1\r\nHOST: fcm.googleapis.com\r\nContent-Type: application/json\r\nAuthorization: key=AAAA8BXZHXk:APA91bHNEW4qP0_TRmoirmFRt9TR80LQyRNBhKiCqAhUyTDAZalx9jwZgn6fHdF5mFRqmaIquZ1kD7fIrohLEb138To4EYlME4irhbrf-RTkQzFWny4Xc0tqBw5jFrSpMnt1rUqZJ29hkT_I7f2vTwxxMLTibmiqTQ\r\nContent-Length: 126\r\n\r\n") +
"{\"notification\":{\"title\": \"PIXEL DETECTED!\",\"body\":\"A Pixel had detected! CHECK NOW!\",\"sound\":\"default\"},\"to\":\"/topics/pixel\"}";

#define COOLOFF_PERIOD 5000

unsigned long _now;
unsigned long _last;
unsigned long _cooloff_timer = 0;

void setup()
{
#ifdef _DEBUG
	Serial.begin(115200);
	Serial.println();
#endif
	
	pinMode(4, OUTPUT);
	pinMode(5, INPUT);

    WiFi.begin("Abramovich", "happylama");

#ifdef _DEBUG
    Serial.print("Connecting");
#endif

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

#ifdef _DEBUG
        Serial.print(".");
#endif
    }

#ifdef _DEBUG
	Serial.println("\nConnected to WiFi!");
#endif
}

void loop()
{
	digitalWrite(4, LOW);
	delayMicroseconds(2);
	digitalWrite(4, HIGH);
	delayMicroseconds(10);
	digitalWrite(4, LOW);
	int duration = pulseIn(5, HIGH);
	int distance = duration/58;

	_now = millis();
	unsigned long dt = _now - _last;
	_last = _now;
	_cooloff_timer += dt;	

	if(distance < 20 && _cooloff_timer >= COOLOFF_PERIOD)
	{
		Alert();
		_cooloff_timer = 0;
	}
	
	delay(100);
}

void Alert()
{
	WiFiClient _client;
	
	if(!_client.connect("fcm.googleapis.com", 80))
	{
#ifdef _DEBUG
		Serial.println("failed to connect");
#endif
		return;
	}

#ifdef _DEBUG
	Serial.println("connected to domain!");
#endif

	_client.print(_request);
}