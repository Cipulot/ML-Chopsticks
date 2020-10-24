/*----------------------------------------------------------------------
 *								ML_CHOPSTICKS
 *
 *	Solution to gather user behavior information when using chopsticks
 *	and compile a set of usage statistics based on a ML generated model.
 *
 *----------------------------------------------------------------------*/

#include <tactigon_led.h>
#include <tactigon_IMU.h>
#include <tactigon_BLE.h>
#include <tactigon_Battery.h>
#include <tactigon_Env.h>
#include <tactigon_UserSerial.h>
#include <stdio.h>
#include <tactigon_IO.h>

T_Led rLed, bLed, gLed;

T_ACC accMeter;
T_AccData accData;

T_GYRO gyroMeter;
T_GyroData gyroData;

T_QUAT qMeter;
T_QData qData;

T_BLE bleManager;
UUID uuid;
T_BLE_Characteristic BLEChar;

uint8_t connStat;

int ticks;

/*----------------------------------------------------------------------*/
void setup()
{
	ticks = 0;

	//init LEDs
	rLed.init(T_Led::RED);
	gLed.init(T_Led::GREEN);
	bLed.init(T_Led::BLUE);
	rLed.off();
	gLed.off();
	bLed.off();

	//init name
	bleManager.setName("CHOP");

	//init role
	bleManager.InitRole(TACTIGON_BLE_PERIPHERAL);

	//add characteristic
	uuid.set("bea5760d-503d-4920-b000-101e7306b005");
	BLEChar = bleManager.addNewChar(uuid, 20);

	//disable use of magnetometer in quaternion computing
	qMeter.enable();
	qMeter.useMag(0);
}

/*----------------------------------------------------------------------*/
void loop()
{

	unsigned char buffData[32];
	char stringBuff[128];
	int cnt;

	//update BLE characteristics @ 50Hz
	if (GetCurrentMilli() >= (ticks + (1000 / 50)))
	{
		ticks = GetCurrentMilli();

		//get data
		accData = accMeter.getAxis();
		gyroData = gyroMeter.getAxis();
		qData = qMeter.getQs();
		int16_t ax, ay, az, gx, gy, gz, r, p, y;

		//inits
		memset(buffData, 0, sizeof(buffData));
		cnt = 0;

		//acc
		ax = (int16_t)(accData.x * 1000);
		memcpy(&buffData[cnt], &ax, 2);
		cnt += 2;

		ay = (int16_t)(accData.y * 1000);
		memcpy(&buffData[cnt], &ay, 2);
		cnt += 2;

		az = (int16_t)(accData.z * 1000);
		memcpy(&buffData[cnt], &az, 2);
		cnt += 2;

		//gyro
		gx = (int16_t)(gyroData.x * 30); //FS=1000  --> max 30000
		memcpy(&buffData[cnt], &gx, 2);
		cnt += 2;

		gy = (int16_t)(gyroData.y * 30);
		memcpy(&buffData[cnt], &gy, 2);
		cnt += 2;

		gz = (int16_t)(gyroData.z * 30);
		memcpy(&buffData[cnt], &gz, 2);
		cnt += 2;

		//euler angles
		r = (int16_t)rollZeroRemap(radToDeg(qData.roll));
		p = (int16_t)radToDeg(qData.pitch);
		y = (int16_t)radToDeg(qData.yaw);

		memcpy(&buffData[cnt], &r, 2);
		cnt += 2;

		memcpy(&buffData[cnt], &p, 2);
		cnt += 2;

		memcpy(&buffData[cnt], &y, 2);
		cnt += 2;

		//update char if connected
		connStat = bleManager.getStatus();
		if (connStat != 0)
		{
			BLEChar.update(buffData);

			sprintf(stringBuff, "A %d %d %d %d %d %d %d %d %d\n", ax, ay, az, gx, gy, gz, r, p, y);
			Serial.print(stringBuff);
		}
	}

	//led handling
	ledHandling();
}

/*----------------------------------------------------------------------*/
void ledHandling()
{
	static int ticksLed = 0;
	static int stp = 0;

	//1 sec loop
	if (millis() >= (ticksLed + 1000))
	{

		ticksLed = millis();

		if (connStat == 0)
		{
			//cycle leds
			if (stp == 0)
			{
				rLed.on();
				gLed.off();
				bLed.off();
			}
			else if (stp == 1)
			{
				rLed.off();
				gLed.on();
				bLed.off();
			}
			else if (stp == 2)
			{
				rLed.off();
				gLed.off();
				bLed.on();
			}
			stp = (stp + 1) % 3;
		}
		else
		{
			//blink green
			if (stp == 0)
			{
				rLed.off();
				gLed.on();
				bLed.off();
			}
			else if (stp == 1)
			{
				rLed.off();
				gLed.off();
				bLed.off();
			}
			stp = (stp + 1) % 3;
		}
	}
}

/*----------------------------------------------------------------------*/
float radToDeg(float rad)
{
	return rad * 180 / PI;
}

/*----------------------------------------------------------------------
 *
 * remap roll in order to have 0 deg in horizontal position
 * and +180,-180 periodicity around this zero position
 *
 *----------------------------------------------------------------------*/
float rollZeroRemap(float r)
{
	if ((r > -180) && (r < 90))
		r = r + 90;
	else
		r = -270 + r;

	return r;
}