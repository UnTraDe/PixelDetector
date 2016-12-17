#include <ESP8266WiFi.h>

#define _DEBUG

#define DISTANCE_THRESHHOLD 20
#define INTERVAL_THRESHHOLD 1000
#define INTERVAL_COUNT_THRESHHOLD 5

#define SLEEP_NO_DETECT 2
#define SLEEP_DETECT 5

#define TRIGGER_PIN 4
#define ECHO_PIN 5

const char _SSID[] = "Abramovich";
const char _PASSWORD[] = "happylama";

String _request = String("POST /fcm/send HTTP/1.1\r\nHOST: fcm.googleapis.com\r\nContent-Type: application/json\r\nAuthorization: key=AAAA8BXZHXk:APA91bHNEW4qP0_TRmoirmFRt9TR80LQyRNBhKiCqAhUyTDAZalx9jwZgn6fHdF5mFRqmaIquZ1kD7fIrohLEb138To4EYlME4irhbrf-RTkQzFWny4Xc0tqBw5jFrSpMnt1rUqZJ29hkT_I7f2vTwxxMLTibmiqTQ\r\nContent-Length: 131\r\n\r\n") +
"{\"notification\":{\"title\": \"PIXEL DETECTED!\",\"body\":\"A Pixel has been detected! CHECK NOW!\",\"sound\":\"default\"},\"to\":\"/topics/pixel\"}";

void setup()
{
#ifdef _DEBUG
	Serial.begin(115200);
	Serial.println();
#endif
	
	pinMode(TRIGGER_PIN, OUTPUT);
	pinMode(ECHO_PIN, INPUT);

	WiFi.mode(WIFI_STA);
	WiFi.begin(_SSID, _PASSWORD);

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
	if(CheckAndVerify())
	{
		Alert();
		ESP.deepSleep(SLEEP_DETECT * 1000000, WAKE_RF_DEFAULT);
	}
	else
	{
		ESP.deepSleep(SLEEP_NO_DETECT * 1000000, WAKE_RF_DEFAULT);
	}
}

bool CheckAndVerify()
{
	int distance = 99999;
	int count = 0;

#ifdef _DEBUG
	Serial.println("checking...");
#endif

	while((distance = GetSample()) <= DISTANCE_THRESHHOLD)
	{
#ifdef _DEBUG
		if(count == 0)
			Serial.print("Verifying");
		
		Serial.print(".");
#endif

		count++;
		delay(INTERVAL_THRESHHOLD);
		
		if(count == INTERVAL_COUNT_THRESHHOLD)
			break;
	}

#ifdef _DEBUG
	Serial.println();
#endif

	if(count == INTERVAL_COUNT_THRESHHOLD)
	{
#ifdef _DEBUG
		Serial.println("detected!");
#endif
		return true;
	}
	else
	{
#ifdef _DEBUG
		Serial.println("not detected");
#endif
		return false;
	}
}

int GetSample()
{
	digitalWrite(TRIGGER_PIN, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIGGER_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIGGER_PIN, LOW);

	return pulseIn(ECHO_PIN, HIGH) / 58;
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

#ifdef _DEBUG
	Serial.println("sent");
#endif
}