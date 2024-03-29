#include <syslog.h>

#include "dht22_emoncms.h"
#include "../emoncms.h"

// from https://github.com/technion/lol_dht22

static int dht22_dat[5] = {0,0,0,0,0};

static uint8_t sizecvt(const int read)
{
  /* digitalRead() and friends from wiringpi are defined as returning a value
  < 256. However, they are returned as int() types. This is a safety function */

  if (read > 255 || read < 0)
  {
    printf("Invalid data from wiringPi library\n");
    exit(EXIT_FAILURE);
  }
  return (uint8_t)read;
}

static int read_dht22_dat(struct config *config, struct data *data)
{
  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;

  dht22_dat[0] = dht22_dat[1] = dht22_dat[2] = dht22_dat[3] = dht22_dat[4] = 0;

  int pDHTpin = config->pDHTpin;

  // pull pin down for 18 milliseconds
  pinMode(pDHTpin, OUTPUT);
  digitalWrite(pDHTpin, HIGH);
  delay(10);
  digitalWrite(pDHTpin, LOW);
  delay(18);
  // then pull it up for 40 microseconds
  digitalWrite(pDHTpin, HIGH);
  delayMicroseconds(40);
  // prepare to read the pin
  pinMode(pDHTpin, INPUT);

  // detect change and read data
  for ( i=0; i< MAXTIMINGS; i++) {
    counter = 0;
    while (sizecvt(digitalRead(pDHTpin)) == laststate) {
      counter++;
      delayMicroseconds(1);
      if (counter == 255) {
        break;
      }
    }
    laststate = sizecvt(digitalRead(pDHTpin));

    if (counter == 255) break;

    // ignore first 3 transitions
    if ((i >= 4) && (i%2 == 0)) {
      // shove each bit into the storage bytes
      dht22_dat[j/8] <<= 1;
      if (counter > 16)
        dht22_dat[j/8] |= 1;
      j++;
    }
  }

  // check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
  // print it out if data is good
  if ((j >= 40) &&
      (dht22_dat[4] == ((dht22_dat[0] + dht22_dat[1] + dht22_dat[2] + dht22_dat[3]) & 0xFF)) ) {
        float t, h;
        h = (float)dht22_dat[0] * 256 + (float)dht22_dat[1];
        h /= 10;
        t = (float)(dht22_dat[2] & 0x7F)* 256 + (float)dht22_dat[3];
        t /= 10.0;
        if ((dht22_dat[2] & 0x80) != 0)  t *= -1;


    printf("Humidity = %.2f %% Temperature = %.2f *C \n", h, t );
    syslog(LOG_INFO, "Humidity = %.2f %% Temperature = %.2f *C \n", h, t );

	data->Humidity = h;
	data->Temperature = t;

	// simple plausibility check
	if (t < -100.0)
	{
		syslog(LOG_INFO, "error: temp < -100.0.");
		printf ("error: temp < -100.0.\n");
		return 0;
	}

    return 1;
  }
  return 0;
}

int gatherData(struct config *config, struct data *data)
// return 0 on setup error or if 10 retries failed
{
  #define MAXTRIES 10

  int i;

  if (wiringPiSetup () == -1)
  {
    printf ("WiringPiSetup() failed.\n");
    return (0);
  }

  for (i=0; i<MAXTRIES; i++)
  {
    if (read_dht22_dat(config, data))
    {
      printf ("h: %5.2f%%\nt: %5.2fC\n", data->Humidity, data->Temperature);
      return 1;
    }
    else
    {
      printf ("Data not good, skip (%d/%d).\n", i+1, MAXTRIES);
      delay (1000);	// wait 1s until retry
    }
  }
  return 0;
}

// send data to emonCMS
int sendToEmonCMS (struct config *config, struct data *data, int socket_fd)
{
    char tcp_buffer[1024];
    int num;

//    printf ("socket_fd: %d\n", socket_fd);

    // generate json string for emonCMS
    sprintf (tcp_buffer, "GET /input/post.json?node=\"%s-env\"&json={Humidity:%4.2f,Temperature:%4.2f}&apikey=%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s %s\r\nConnection: keep-alive\r\n\r\n", config->pNodeName, data->Humidity, data->Temperature, config->pApiKey, config->pHostName, TOOLNAME, DHT22_VERSION);

    printf ("-----\nbuflen: %ld\n%s\n", strlen(tcp_buffer), tcp_buffer);
    printf ("sent: %ld\n", send(socket_fd, tcp_buffer, strlen(tcp_buffer), 0));

    return 1;   // 0 - fail; 1 - success
}
